from pydantic import BaseModel

from schemas.constraints.file import BucketConstraint, KeyConstraint
from schemas.file import FileCreate
from schemas.render import RenderCreate


class GenerateRenderEvent(RenderCreate):
    """
    Схема для события генерация проекта.
    """

    bucket: BucketConstraint
    key: KeyConstraint
    project_id: int


class AddRenderProjectEvent(BaseModel):
    """
    Схема для события загрузка готового рендера в проект.
    """

    file: FileCreate
    project_id: int
