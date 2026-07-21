from abc import ABC, abstractmethod
from typing import BinaryIO


class AbstractS3Client(ABC):
    """
    Интерфейс для работы с s3 хранилищем.
    """

    @abstractmethod
    async def get_object(self, bucket: str, key: str) -> BinaryIO:
        """
        Метод для получения файла из s3 хранилища.
        """

    @abstractmethod
    async def put_object(self, bucket: str, key: str, file: BinaryIO) -> None:
        """
        Метод для загрузки файла в s3 хранилище.
        """

    @abstractmethod
    async def delete_object(self, bucket: str, key: str) -> None:
        """
        Метод для удаления файла из s3 хранилища.
        """
