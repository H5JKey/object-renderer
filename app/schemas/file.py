from pydantic import BaseModel


class FileBase(BaseModel):
    """
    Базовая схема для работы с фалами.
    """

    bucket: str
    key: str


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
