from abc import ABC, abstractmethod
from typing import BinaryIO


class AbstractS3Client(ABC):
    @abstractmethod
    async def get_object(self, bucket: str, key: str) -> None:
        """
        Метод для получения файла из s3 хранилища.
        """

    @abstractmethod
    async def put_object(self, bucket: str, key: str, file: BinaryIO) -> BinaryIO:
        """
        Метод для загрузки файла в s3 хранилище.
        """

    @abstractmethod
    async def delete_object(self, bucket: str, key: str) -> None:
        """
        Метод для удаления файла из s3 хранилища.
        """


class AbstractModelUploader(ABC):
    @abstractmethod
    async def upload_model(
        self,
        bucket: str,
        key: str,
        file: BinaryIO,
    ) -> None:
        """
        Метод для загрузки модели.
        """
