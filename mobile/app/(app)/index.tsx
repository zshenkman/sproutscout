import { useState, useEffect } from 'react'
import { supabase } from '../../lib/supabase'
import { StyleSheet, View, Alert, FlatList, ListRenderItem, ListRenderItemInfo, Image } from 'react-native'
import { useSession } from '../../contexts/SessionContext'
import { Text } from '../../components/Themed'

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
    return (
      <View style={styles.plantCardContainer}>
        <Image source={require('../../assets/images/plant.png')} style={styles.plantCardImage} resizeMode={'contain'}/>
        <Text style={styles.plantCardTitleText}>{item.name}</Text>
        <Text style={styles.plantCardText}>Temperature: <Text style={styles.plantCardValueText}>{Number(item.temperature).toPrecision(3)}° F</Text></Text>
        <Text style={styles.plantCardText}>Humidity: <Text style={styles.plantCardValueText}>{Number(item.humidity).toPrecision(3)}%</Text></Text>
        <Text style={styles.plantCardText}>Soil Moisture: <Text style={styles.plantCardValueText}>{item.soil_moisture} mL</Text></Text>
        <Text style={styles.plantCardText}>Light: <Text style={styles.plantCardValueText}>{item.light}</Text></Text>
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
  }
})