from typing import ClassVar

from core.constants import ProjectVisibility, RenderStatus
from pydantic import BaseModel, ConfigDict

from schemas.render import RenderCreate, RenderResponse


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

    visibility: ProjectVisibility
    status: RenderStatus
    render_id: int | None
    id: int


class ProjectResponseList(BaseModel):
    """
    Схема для вывода информации о списке проектов.
    """

    project_list: list[ProjectResponse]
    size: int
    page: int


class ProjectWithRenderCreate(BaseModel):
    """
    Схема для создания проекта c предоставлением данных о рендере.
    """

    render: RenderCreate
    project: ProjectCreate


class ProjectWithRenderResponse(ProjectResponse):
    """
    Схема для вывода информации о проекте с рендером.
    """

    render: RenderResponse


class ProjectWithRenderResponseList(BaseModel):
    """
    Схема для вывода информации о списке проектов с рендером.
    """

    project_list: list[ProjectWithRenderResponse]
    size: int
    page: int
