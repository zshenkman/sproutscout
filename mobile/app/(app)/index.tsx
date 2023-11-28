import { useState, useEffect } from 'react'
import { supabase } from '../../lib/supabase'
import { StyleSheet, View, Alert, FlatList, ListRenderItemInfo, Image, TouchableOpacity } from 'react-native'
import { useSession } from '../../contexts/SessionContext'
import { Text } from '../../components/Themed'
import { FontAwesomeIcon } from '@fortawesome/react-native-fontawesome'
import { faDroplet, faDropletSlash, faLightbulb, faLightbulbSlash } from '@fortawesome/pro-regular-svg-icons'

export default function Home() {
  const { session } = useSession()
  const [loading, setLoading] = useState<boolean>(true)
  const [plants, setPlants] = useState<any[]>([])

  useEffect(() => {
    fetchPlants()
  }, [])

  useEffect(() => {
    const listener = supabase
      .channel('schema-db-changes')
      .on('postgres_changes', { event: 'UPDATE', schema: 'public', table: 'plants' }, payload => {
        const updatedPlant = payload.new
        const newPlants = Array.from(plants).map((plant: any) => {
          if (plant.id === updatedPlant.id) {
            return updatedPlant
          } else {
            return plant
          }
        })
        setPlants(newPlants)
      })

    listener.subscribe()

    return () => {
      listener.unsubscribe()
    }
  }, [plants])

  async function fetchPlants() {
    try {
      setLoading(true)

      const { data, error, status } = await supabase
        .from('plants')
        .select(`*`)
        .eq('owner_id', session?.user.id)
        .order('created_at', {ascending: false})

      if (error && status !== 406) {
        throw error
      }

      if (data) {
        setPlants(data)
      }
    } catch (error) {
      if (error instanceof Error) {
        Alert.alert(error.message)
      }
    } finally {
      setLoading(false)
    }
  }

  function renderPlantCard({ item }: ListRenderItemInfo<any>) {
    const lightDescription = (() => {
      if (item.light <= 1500) {
        return "Dark"
      }
      else if (item.light <= 3000) {
        return "Shady"
      }
      else if (item.light > 3000) {
        return "Bright"
      }
    })()

    const moistureDescription = (() => {
      if (item.soil_moisture <= 1500) {
        return "Dry"
      }
      else if (item.soil_moisture <= 3000) {
        return "Damp"
      }
      else if (item.soil_moisture > 3000) {
        return "Wet"
      }
    })()

    const toggleLight = async () => {
      await supabase
        .from('plants')
        .update({ light_enabled: !item.light_enabled })
        .eq('id', item.id)
      item.light_enabled = !item.light_enabled
    }

    const toggleWater = async () => {
      await supabase
        .from('plants')
        .update({ water_enabled: !item.water_enabled })
        .eq('id', item.id)
      item.water_enabled = !item.water_enabled
    }

    return (
      <View style={styles.plantCardContainer}>
        <Image source={require('../../assets/images/plant.png')} style={styles.plantCardImage} resizeMode={'contain'}/>
        <Text style={styles.plantCardTitleText}>{item.name}</Text>
        <Text style={styles.plantCardText}>Temperature: <Text style={styles.plantCardValueText}>{Number(item.temperature).toPrecision(3)}° F</Text></Text>
        <Text style={styles.plantCardText}>Humidity: <Text style={styles.plantCardValueText}>{Number(item.humidity).toPrecision(3)}%</Text></Text>
        <Text style={styles.plantCardText}>Soil Moisture: <Text style={styles.plantCardValueText}>{moistureDescription} ({item.soil_moisture} mL)</Text></Text>
        <Text style={styles.plantCardText}>Light: <Text style={styles.plantCardValueText}>{lightDescription}</Text></Text>
        <View style={styles.plantCardButtonsContainer}>
          <TouchableOpacity style={styles.plantCardButton} onPress={toggleLight}>
            <FontAwesomeIcon icon={item.light_enabled ? faLightbulbSlash : faLightbulb} color={'#fff'} size={18}/>
          </TouchableOpacity>
          <View style={styles.buttonSeperator}/>
          <TouchableOpacity style={styles.plantCardButton} onPress={toggleWater}>
            <FontAwesomeIcon icon={item.water_enabled ? faDropletSlash : faDroplet} color={'#fff'} size={18}/>
          </TouchableOpacity>
        </View>
      </View>
    )
  }

  return (
    <View style={styles.container}>
      <FlatList
        contentContainerStyle={styles.listContent}
        data={plants}
        renderItem={renderPlantCard}
        refreshing={loading}
        onRefresh={fetchPlants}
      />
    </View>
  )
}

const styles = StyleSheet.create({
  container: {
    flex: 1
  },
  listContent: {
    padding: 15,
    gap: 15
  },
  plantCardContainer: {
    paddingHorizontal: 20,
    paddingVertical: 20,
    borderRadius: 15,
    backgroundColor: '#fff',
    gap: 10
  },
  plantCardImage: {
    width: 60,
    height: 60
  },
  plantCardTitleText: {
    fontSize: 20,
    fontWeight: '600'
  },
  plantCardText: {
    fontSize: 16
  },
  plantCardValueText: {
    fontWeight: '600'
  },
  plantCardButtonsContainer: {
    position: 'absolute',
    top: 20,
    right: 20,
    borderRadius: 10,
    flexDirection: 'row',
    alignItems: 'center',
    backgroundColor: '#009E60',
    overflow: 'hidden'
  },
  plantCardButton: {
    padding: 10
  },
  buttonSeperator: {
    height: '70%',
    width: 1,
    backgroundColor: '#fff'
  }
})