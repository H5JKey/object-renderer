from typing import ClassVar

from core.constants import ProjectVisibility, RenderStatus
from pydantic import BaseModel, ConfigDict


class RenderProjectBase(BaseModel):
    """
    Базовая схема для создания проекта.
    """

    name: str
    description: str
    source_file_id: int
    model_config: ClassVar[ConfigDict] = ConfigDict(from_attributes=True)


class RenderProjectCreate(RenderProjectBase):
    """
    Схема для создания проекта.
    """

    visibility: ProjectVisibility


class RenderProjectResponse(RenderProjectBase):
    """
    Схема для вывода информации о проекте.
    """

    render_file_id: int | None = None
    visibility: ProjectVisibility
    status: RenderStatus
    id: int


class RenderProjectResponseList(BaseModel):
    """
    Схема для вывода информации о списке проектов.
    """

    render_projects: list[RenderProjectResponse]
    size: int
    page: int
