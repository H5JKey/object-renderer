from core.constants import ProjectVisibility
from pydantic import BaseModel


class RenderProjectBase(BaseModel):
    """
    Базовая схема для создания проекта.
    """

    name: str
    description: str


class RenderProjectCreate(RenderProjectBase):
    """
    Схема для создания проекта.
    """

    source_file_id: int
    visibility: ProjectVisibility


class RenderProjectResponse(RenderProjectBase):
    """
    Схема для вывода информации о проекте.
    """

    render_file_id: int | None
    visibility: ProjectVisibility
    id: int
