# SPDX-License-Identifier: BSD-2-Clause
#
# This file is part of pyosmium. (https://osmcode.org/pyosmium/)
#
# Copyright (C) 2024 Sarah Hoffmann <lonvia@denofr.de> and others.
# For a full list of authors see the git log.

from osmium._osmium import apply, NodeLocationsForWays
from osmium.io import Reader, FileBuffer
from osmium.osm import osm_entity_bits
from osmium.area import AreaManager
from osmium.index import create_map

class SimpleHandler:
    """ The most generic of OSM data handlers. Derive your data processor
        from this class and implement callbacks for each object type you are
        interested in. The following data types are recognised:

          `node`, `way`, `relation`, `area` and `changeset`

        A callback takes exactly one parameter which is the object. Note that
        all objects that are handed into the handler are only readable and are
        only valid until the end of the callback is reached. Any data that
        should be retained must be copied into other data structures.
    """
    def enabled_for(self):
        """ Return the list of OSM object types this handler will handle.
        """
        entities = osm_entity_bits.NOTHING
        if hasattr(self, 'node'):
            entities |= osm_entity_bits.NODE
        if hasattr(self, 'way'):
            entities |= osm_entity_bits.WAY
        if hasattr(self, 'relation'):
            entities |= osm_entity_bits.RELATION
        if hasattr(self, 'area'):
            entities |= osm_entity_bits.AREA
        if hasattr(self, 'changeset'):
            entities |= osm_entity_bits.CHANGESET

        return entities

    def apply_file(self, filename, locations=False, idx='flex_mem', filters=[]):
        """ Apply the handler to the given file. If locations is true, then
            a location handler will be applied before, which saves the node
            positions. In that case, the type of this position index can be
            further selected in idx. If an area callback is implemented, then
            the file will be scanned twice and a location handler and a
            handler for assembling multipolygons and areas from ways will
            be executed.
        """
        self._apply_object(str(filename), locations, idx, filters)


    def apply_buffer(self, buffer, format, locations=False, idx='flex_mem', filters=[]):
        """Apply the handler to a string buffer. The buffer must be a
           byte string.
        """
        self._apply_object(FileBuffer(buffer, format), locations, idx, filters)


    def _apply_object(self, obj, locations, idx, filters):
        entities = self.enabled_for()
        if entities & osm_entity_bits.AREA:
            area = AreaManager()
            rd = Reader(obj, osm_entity_bits.RELATION)
            try:
                apply(rd, *filters, area.first_pass_handler())
            finally:
                rd.close()

            entities |= osm_entity_bits.OBJECT
            lh = NodeLocationsForWays(create_map(idx))
            lh.ignore_errors()
            handlers = [lh, area.second_pass_handler(*filters, self), *filters, self]
        elif locations:
            entities |= osm_entity_bits.NODE
            lh = NodeLocationsForWays(create_map(idx))
            lh.ignore_errors()
            handlers = [lh, *filters, self]
        else:
            handlers = [*filters, self]

        rd = Reader(obj, entities)
        try:
            apply(rd, *handlers)
        finally:
            rd.close()
