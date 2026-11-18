idf_component_register(
    SRCS 
        "Drawable.cpp"
        "Line.cpp"
        "Rectangle.cpp"
        "Circle.cpp"
        "Triangle.cpp"
    INCLUDE_DIRS "."
    REQUIRES ili9341_display
)