from abc import ABC, abstractmethod
from typing import BinaryIO

from models import File
from schemas.file import FileCreate


class AbstractFileRepository(ABC):
    @abstractmethod
    async def get_by_id(self, file_id: int) -> File | None:
        """
        Метод для получения данных о файле по id.
        """

    @abstractmethod
    async def create_file(self, user_id: int, create_file_data: FileCreate) -> File:
        """
        Метод для создания записи о файле.
        """

    @abstractmethod
    async def delete_by_id(self, file_id: int) -> None:
        """
        Метод для удаления данных о файле.
        """


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


class AbstractFileUploader(ABC):
    @abstractmethod
    async def upload(
        self,
        user_id: int,
        file_name: str,
        file: BinaryIO,
    ) -> None:
        """
        Метод для загрузки файла.
        """
