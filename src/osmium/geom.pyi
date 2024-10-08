# SPDX-License-Identifier: BSD-2-Clause
#
# This file is part of pyosmium. (https://osmcode.org/pyosmium/)
#
# Copyright (C) 2024 Sarah Hoffmann <lonvia@denofr.de> and others.
# For a full list of authors see the git log.
from typing import ClassVar, Union

from typing import overload
import osmium.osm
ALL: use_nodes
BACKWARD: direction
FORWARD: direction
UNIQUE: use_nodes

PointLike = Union[osmium.osm.Node, osmium.osm.Location, osmium.osm.NodeRef]
LineStringLike = Union[osmium.osm.Way, osmium.osm.WayNodeList]

class use_nodes:
    ALL: ClassVar[use_nodes] = ...
    UNIQUE: ClassVar[use_nodes] = ...
    def __init__(self, value: int) -> None: ...
    @property
    def name(self) -> str: ...
    @property
    def value(self) -> int: ...

class direction:
    BACKWARD: ClassVar[direction] = ...
    FORWARD: ClassVar[direction] = ...
    def __init__(self, value: int) -> None: ...
    @property
    def name(self) -> str: ...
    @property
    def value(self) -> int: ...

class Coordinates:
    @overload
    def __init__(self) -> None: ...
    @overload
    def __init__(self, cx: float, cy: float) -> None: ...
    @overload
    def __init__(self, location: osmium.osm.Location) -> None: ...
    def valid(self) -> bool: ...
    @property
    def x(self) -> float: ...
    @property
    def y(self) -> float: ...

class FactoryProtocol:
    def __init__(self) -> None: ...
    def create_linestring(self, line: LineStringLike, use_nodes: use_nodes = ..., direction: direction = ...) -> str: ...
    def create_multipolygon(self, area: osmium.osm.Area) -> str: ...
    def create_point(self, location: PointLike) -> str: ...
    @property
    def epsg(self) -> int: ...
    @property
    def proj_string(self) -> str: ...

class GeoJSONFactory(FactoryProtocol):
    pass

class WKBFactory(FactoryProtocol):
    pass

class WKTFactory(FactoryProtocol):
    pass

def haversine_distance(list: osmium.osm.WayNodeList) -> float: ...
def lonlat_to_mercator(coordinate: Coordinates) -> Coordinates: ...
def mercator_to_lonlat(coordinate: Coordinates) -> Coordinates: ...
