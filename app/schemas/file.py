from typing import ClassVar

from pydantic import BaseModel, ConfigDict


class FileBase(BaseModel):
    """
    Базовая схема для работы с фалами.
    """

    bucket: str
    key: str

    model_config: ClassVar[ConfigDict] = ConfigDict(from_attributes=True)


class FileCreate(FileBase):
    """
    Схема для создания файла.
    """


class FileResponse(FileBase):
    """
    Схема для вывода информации о файле.
    """

    id: int
    user_id: int
