from abc import ABC, abstractmethod

from models import File, User
from schemas.file import FileCreate
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
