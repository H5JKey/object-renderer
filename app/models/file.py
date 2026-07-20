from typing import TYPE_CHECKING

from core.constants import FILE_BUCKET_LENGTH, FILE_KEY_LENGTH
from infrastructure.database import Base
from sqlalchemy import ForeignKey, String
from sqlalchemy.orm import Mapped, mapped_column, relationship

if TYPE_CHECKING:
    from models import User


class File(Base):
    __tablename__ = "files"

    id: Mapped[int] = mapped_column(primary_key=True)
    bucket: Mapped[str] = mapped_column(String(FILE_BUCKET_LENGTH))
    key: Mapped[str] = mapped_column(String(FILE_KEY_LENGTH))
    user_id: Mapped[int] = mapped_column(ForeignKey("users.id"))

    user: Mapped["User"] = relationship(
        "User",
        back_populates="files",
    )
