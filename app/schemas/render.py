from typing import ClassVar

from pydantic import BaseModel, ConfigDict

from schemas.constraints.file import BucketConstraint, KeyConstraint
from schemas.constraints.render import (
    HeightConstraint,
    SampleConstraint,
    WidthConstraint,
)
from schemas.file import FileCreate, FileResponse


class RenderBase(BaseModel):
    """
    Базовая схема для рендера.
    """

    width: WidthConstraint
    height: HeightConstraint
    samples: SampleConstraint
    denoiser: bool
    gpu: bool

    model_config: ClassVar[ConfigDict] = ConfigDict(from_attributes=True)


class RenderCreate(RenderBase):
    """
    Схема для создания рендера.
    """


class RenderResponse(RenderBase):
    """
    Схема для вывода информации о рендере.
    """

    file_id: int | None
    id: int


class RenderWithFileResponse(RenderResponse):
    """
    Схема для вывода информации о рендере вместе с файлом.
    """

    file: FileResponse | None


class GenerateRenderEvent(RenderCreate):
    """
    Схема для события генерация проекта.
    """

    bucket: BucketConstraint
    key: KeyConstraint
    project_id: int


class UploadRenderProjectEvent(BaseModel):
    """
    Схема для события загрузка рендер модели.
    """

    project_id: int
    file: FileCreate
