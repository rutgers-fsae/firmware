import re


def slugify(name: str) -> str:
    slug = re.sub(r"[^a-zA-Z0-9]+", "-", name.strip().lower()).strip("-")
    return slug or "dataset"


def unique_slug(base_slug: str, existing_slugs: set[str]) -> str:
    if base_slug not in existing_slugs:
        return base_slug
    index = 2
    while True:
        candidate = f"{base_slug}-{index}"
        if candidate not in existing_slugs:
            return candidate
        index += 1
