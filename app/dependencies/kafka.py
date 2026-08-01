from aiokafka import AIOKafkaProducer
from infrastructure.kafka import producer


async def get_aiokafka_producer() -> AIOKafkaProducer:
    return producer
