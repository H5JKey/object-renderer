from typing import ClassVar

from core.constants import ProjectVisibility, RenderStatus
from pydantic import BaseModel, ConfigDict


class ProjectBase(BaseModel):
    """
    Базовая схема для создания проекта.
    """

    name: str
    description: str
    source_file_id: int
    model_config: ClassVar[ConfigDict] = ConfigDict(from_attributes=True)


class ProjectCreate(ProjectBase):
    """
    Схема для создания проекта.
    """

    visibility: ProjectVisibility


class ProjectResponse(ProjectBase):
    """
    Схема для вывода информации о проекте.
    """

    render_file_id: int | None = None
    visibility: ProjectVisibility
    status: RenderStatus
    id: int


class ProjectResponseList(BaseModel):
    """
    Схема для вывода информации о списке проектов.
    """

    project_list: list[ProjectResponse]
    size: int
    page: int
