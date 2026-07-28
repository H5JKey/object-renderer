from typing import TYPE_CHECKING

from core.constants import FILE_BUCKET_LENGTH, FILE_KEY_LENGTH
from infrastructure.database import Base
from sqlalchemy import String
from sqlalchemy.orm import Mapped, mapped_column, relationship

if TYPE_CHECKING:
    from models import Project


class File(Base):
    __tablename__ = "files"

    id: Mapped[int] = mapped_column(primary_key=True)
    bucket: Mapped[str] = mapped_column(String(FILE_BUCKET_LENGTH))
    key: Mapped[str] = mapped_column(String(FILE_KEY_LENGTH))

    project_as_source_file: Mapped["Project"] = relationship(
        "Project",
        foreign_keys="Project.source_file_id",
        back_populates="source_file",
    )
    project_as_rendered_file: Mapped["Project"] = relationship(
        "Project",
        foreign_keys="Project.rendered_file_id",
        back_populates="rendered_file",
    )
