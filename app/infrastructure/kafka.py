import asyncio
from collections.abc import Callable

from confluent_kafka import Consumer, Producer
from core.interfaces.kafka import KafkaConsumer, KafkaProducer


class ConfluentKafkaProducer(KafkaProducer):
    def __init__(
        self,
        config: dict,  # type: ignore[type-arg]
    ) -> None:
        self._producer = Producer(config)

    def produce(
        self,
        topic: str,
        value: str | bytes | None = None,
        key: str | bytes | None = None,
        partition: int = -1,
        callback: Callable | None = None,  # type: ignore[type-arg]
        on_delivery: Callable | None = None,  # type: ignore[type-arg]
        timestamp: int = 0,
    ) -> None:
        self._producer.produce(
            topic=topic,
            value=value,
            key=key,
            partition=partition,
            callback=callback,
            on_delivery=on_delivery,
            timestamp=timestamp,
        )

    async def flush(self) -> None:
        await asyncio.to_thread(self._producer.flush)


class ConfluentKafkaConsumer(KafkaConsumer):
    def __init__(
        self,
        config: dict,  # type: ignore[type-arg]
    ) -> None:
        self._consumer = Consumer(config)

    def subscribe(self, topics: list[str]) -> None:
        self._consumer.subscribe(topics=topics)

    async def poll(self, timeout: int = 0) -> None:  # noqa: ASYNC109
        await asyncio.to_thread(self._consumer.poll, timeout)
