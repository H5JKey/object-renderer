from abc import ABC, abstractmethod
from collections.abc import Callable


class KafkaProducer(ABC):
    """
    Интерфейс для реализации продюсера кафка брокера.
    """

    @abstractmethod
    def produce(
        self,
        topic: str,
        value: str | bytes | None,
        key: str | bytes | None,
        partition: int,
        callback: Callable | None,  # type:ignore[type-arg]
        on_delivery: Callable | None,  # type:ignore[type-arg]
        timestamp: int,
    ) -> None:
        """
        Метод для отправки сообщения в буфер.
        """

    @abstractmethod
    async def flush(self) -> None:
        """
        Метод для отправки всех текущих сообщений.
        """


class KafkaConsumer(ABC):
    """
    Интерфейс для реализации консьюмера кафка брокера.
    """

    @abstractmethod
    async def subscribe(self, topics: list[str]) -> None:
        """
        Метод для подписки консьюмера на события в списке топиков.
        """

    @abstractmethod
    async def poll(
        self,
        timeout: int | None = None,  # noqa:ASYNC109
    ) -> None:
        """
        Получение сообщений с таймаутом в случае пустых топиков.
        """
