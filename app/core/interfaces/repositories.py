from abc import ABC, abstractmethod

from models import File, RenderProject, User
from pydantic import EmailStr
from schemas.file import FileCreate
from schemas.render_project import RenderProjectCreate
from schemas.user import UserCreate


class AbstractFileRepository(ABC):
    """
    Интерфейс репозитория для работы с файлами.
    """

    @abstractmethod
    async def get_by_id(self, file_id: int) -> File | None:
        """
        Метод для получения данных о файле по id.
        """

    @abstractmethod
    async def create_file(self, create_file_data: FileCreate) -> File:
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
    Интерфейс репозитория для работы с пользователями.
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
    async def get_by_email(self, email: EmailStr) -> User | None:
        """
        Метод для поиска пользователя по email.
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


class AbstractRenderProjectRepository(ABC):
    """
    Интерфейс репозитория для работы с проектами.
    """

    @abstractmethod
    async def get_by_id(self, render_project_id: int) -> RenderProject | None:
        """
        Получить информацию о проекте.
        """

    @abstractmethod
    async def get_render_project_owner(self, render_project_id: int) -> User | None:
        """
        Получить информацию о владельце проекта.
        """

    @abstractmethod
    async def get_user_render_projects(self, user_id: int) -> list[RenderProject]:
        """
        Посмотреть список проектов пользователя.
        """

    @abstractmethod
    async def get_user_public_render_projects(
        self,
        user_id: int,
    ) -> list[RenderProject]:
        """
        Получить информацию о публичных проектах пользователя.
        """

    @abstractmethod
    async def create_render_project(
        self,
        user_id: int,
        create_render_project_data: RenderProjectCreate,
    ) -> RenderProject:
        """
        Создать проект.
        """

    @abstractmethod
    async def delete_by_id(self, render_project_id: int) -> None:
        """
        Удалить проект по id.
        """
