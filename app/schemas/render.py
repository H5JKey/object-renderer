from pydantic import BaseModel

from schemas.constraints import FovConstraint, SampleConstraint, SizeConstraint


class RenderBase(BaseModel):
    """
    Базовая схема для рендера.
    """

    width: SizeConstraint
    height: SizeConstraint
    samples: SampleConstraint
    use_denoiser: bool
    fov: FovConstraint
    camera_coordinates: tuple[float, float, float]
    camera_view_coordinates: tuple[float, float, float]


class RenderCreate(RenderBase):
    """
    Схема для создания рендера.
    """

    width: SizeConstraint = 600
    height: SizeConstraint = 800
    samples: SampleConstraint = 20
    use_denoiser: bool = True
    fov: FovConstraint = 90
    camera_coordinates: tuple[float, float, float] = (100, 100, 100)
    camera_view_coordinates: tuple[float, float, float] = (0, 0, 0)


class RenderResponse(RenderBase):
    """
    Схема для вывода информации о рендере.
    """
