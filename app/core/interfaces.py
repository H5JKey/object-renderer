from abc import ABC, abstractmethod
from typing import BinaryIO

from models import File, User
from schemas.auth import LoginRequest, RegisterRequest
from schemas.file import FileCreate
from schemas.token import TokenInfo
from schemas.user import UserCreate


class AbstractFileRepository(ABC):
    """
    Интерфейс для репозитория работы с файлами в базе данных.
    """

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
    """
    Интерфейс для работы с s3 хранилищем.
    """

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
    """
    Интерфейс для сервиса загрузки файлов.
    """

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


class AbstractUserRepository(ABC):
    """
    Интерфейс для репозитория работы с пользователями.
    """

    @abstractmethod
    async def get_by_id(self, user_id: int) -> User | None:
        """
        Метод для поиска пользователя по id.
        """

    @abstractmethod
    async def get_by_username(self, username: str) -> User | None:
        """
        Метод для поиска пользователя по username.
        """

    @abstractmethod
    async def create_user(self, create_user_data: UserCreate) -> User:
        """
        Метод для создания пользователя.
        """

    @abstractmethod
    async def delete_by_id(self, user_id: int) -> None:
        """
        Метод для удаления пользователя по id.
        """


class AbstractAuthService(ABC):
    """
    Интерфейс для сервиса аутентификации / авторизации.
    """

    @abstractmethod
    async def register(
        self,
        register_user_data: RegisterRequest,
    ) -> TokenInfo:
        """
        Регистрация пользователя.
        """

    @abstractmethod
    async def authenticate_user(
        self,
        auth_user_data: LoginRequest,
    ) -> TokenInfo:
        """
        Вход пользователя в приложение.
        """

    @abstractmethod
    async def refresh_access_token(self, refresh_token: str) -> TokenInfo:
        """
        Обновление токена доступа.
        """
