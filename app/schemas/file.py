from typing import ClassVar

from pydantic import BaseModel, ConfigDict


class FileBase(BaseModel):
    """
    Базовая схема для работы с фалами.
    """

    model_config: ClassVar[ConfigDict] = ConfigDict(from_attributes=True)


class FileCreate(FileBase):
    """
    Схема для создания файла.
    """

    bucket: str
    key: str


class FileResponse(FileBase):
    """
    Схема для вывода информации о файле.
    """

    id: int
