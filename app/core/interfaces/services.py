from abc import ABC, abstractmethod
from typing import BinaryIO

from schemas.file import FileResponse
from schemas.render import RenderCreate


class AbstractFileUploader(ABC):
    """
    Интерфейс для сервиса загрузки файлов.
    """

    @abstractmethod
    async def upload(
        self,
        file_name: str,
        file: BinaryIO,
    ) -> FileResponse:
        """
        Метод для загрузки файла.
        """


class AbstractRenderService(ABC):
    """
    Интерфейс для рендера моделей.
    """

    @abstractmethod
    async def send_render_model_event(self, create_render_data: RenderCreate) -> None:
        """
        Метод для создания моделей.
        """
