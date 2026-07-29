from abc import ABC, abstractmethod

from infrastructure.database.models import File, Project, Render, User
from pydantic import EmailStr
from schemas.file import FileCreate
from schemas.project import ProjectCreate
from schemas.render import RenderCreate
from schemas.user import UserCreate, UserUpdate


class AbstractRepository(ABC):  # noqa: B024
    """
    Родительский класс-интерфейс, от которого наследуются другие репозитории.
    """


class AbstractRenderRepository(AbstractRepository):
    """
    Интерфейс для репозитория рендеров.
    """

    @abstractmethod
    async def create_render(self, create_render_data: RenderCreate) -> Render:
        """
        Метод для создания записи о рендере.
        """


class AbstractFileRepository(AbstractRepository):
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


class AbstractUserRepository(AbstractRepository):
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
    async def update_user(
        self,
        user_id: int,
        update_user_data: UserUpdate,
    ) -> User | None:
        """
        Метод для обновления данных о пользователе.
        """

    @abstractmethod
    async def delete_by_id(self, user_id: int) -> None:
        """
        Метод для удаления пользователя по id.
        """


class AbstractProjectRepository(AbstractRepository):
    """
    Интерфейс репозитория для работы с проектами.
    """

    @abstractmethod
    async def get_by_id(self, project_id: int) -> Project | None:
        """
        Получить информацию о проекте.
        """

    @abstractmethod
    async def get_project_owner(self, project_id: int) -> User | None:
        """
        Получить информацию о владельце проекта.
        """

    @abstractmethod
    async def get_user_projects(self, user_id: int) -> list[Project]:
        """
        Посмотреть список проектов пользователя.
        """

    @abstractmethod
    async def get_user_public_projects(
        self,
        user_id: int,
    ) -> list[Project]:
        """
        Получить информацию о публичных проектах пользователя.
        """

    @abstractmethod
    async def create_project(
        self,
        user_id: int,
        render_id: int,
        create_project_data: ProjectCreate,
    ) -> Project:
        """
        Создать проект.
        """

    @abstractmethod
    async def delete_by_id(self, project_id: int) -> None:
        """
        Удалить проект по id.
        """
