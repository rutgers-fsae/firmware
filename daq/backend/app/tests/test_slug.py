from app.core.slug import slugify, unique_slug


def test_slugify():
    assert slugify("May_30_2026_Peter") == "may-30-2026-peter"


def test_unique_slug_collision():
    result = unique_slug("dataset", {"dataset", "dataset-2"})
    assert result == "dataset-3"
