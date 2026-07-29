from collections.abc import AsyncGenerator
from typing import Annotated

from fastapi import Depends
from infrastructure.kafka import ConfluentKafkaProducer, get_producer_config


async def get_confluent_kafka_producer(
    config: Annotated[  # type: ignore[type-arg]
        dict,
        Depends(get_producer_config),
    ],
) -> AsyncGenerator[ConfluentKafkaProducer]:
    producer = ConfluentKafkaProducer(config)
    yield producer
