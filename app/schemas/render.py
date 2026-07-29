from typing import ClassVar

from pydantic import BaseModel, ConfigDict

from schemas.constraints.render import (
    HeightConstraint,
    SampleConstraint,
    WidthConstraint,
)


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
