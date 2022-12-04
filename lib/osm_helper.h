#ifndef PYOSMIUM_OSM_HELPER_HPP
#define PYOSMIUM_OSM_HELPER_HPP

template <typename T>
class CNodeRefList {
    using ImplClass = T const;

public:
    CNodeRefList(ImplClass *list) : m_obj(list) {}

    ImplClass *get() const {
        return m_obj;
    }

    pybind11::object get_item(Py_ssize_t idx) const
     {
        auto sz = m_obj->size();

        osmium::NodeRefList::size_type iout =
            (idx >= 0 ? idx : (Py_ssize_t) sz + idx);

        if (iout >= sz || iout < 0) {
            throw pybind11::index_error("Bad index.");
        }

        auto const &node = (*m_obj)[iout];

        static auto node_ref_t = pybind11::module_::import("osmium.osm.types").attr("NodeRef");

        return node_ref_t(node.location(), node.ref());
     }

private:
    ImplClass *m_obj;
};

using CWayNodeList = CNodeRefList<osmium::WayNodeList>;
using COuterRing = CNodeRefList<osmium::OuterRing>;
using CInnerRing = CNodeRefList<osmium::InnerRing>;

template <typename T, typename C>
class GenericIterator
{
    using Container = C;
    using Base = T;

public:
    GenericIterator(Container const &t)
    : m_it(t.cbegin()), m_cend(t.cend()), m_size(t.size())
    {}

    pybind11::object next()
    {
        if (m_it == m_cend)
            throw pybind11::stop_iteration();

        auto value = static_cast<Base const *>(this)->to_value();
        ++m_it;

        return value;
    }

    int size() const { return m_size; }

protected:
    typename Container::const_iterator m_it;

private:
    typename Container::const_iterator const m_cend;
    int const m_size;
};


class TagListIterator : public GenericIterator<TagListIterator, osmium::TagList>
{
    using GenericIterator::GenericIterator;

public:
    pybind11::object to_value() const {
        static auto tag = pybind11::module_::import("osmium.osm.types").attr("Tag");
        return tag(m_it->key(), m_it->value());
    }
};


class RelationMemberListIterator: public GenericIterator<RelationMemberListIterator, osmium::RelationMemberList>
{
    using GenericIterator::GenericIterator;

public:
    pybind11::object to_value() const {
        static auto memb = pybind11::module_::import("osmium.osm.types").attr("RelationMember");
        return memb(m_it->ref(), item_type_to_char(m_it->type()), m_it->role());
    }
};


template <typename T>
class RingIterator {
    using IteratorType = osmium::memory::ItemIteratorRange<T const>;
public:
    RingIterator(IteratorType t)
    : m_it(t.cbegin()), m_cend(t.cend())
    {}

    CNodeRefList<T> next()
    {
        if (m_it == m_cend)
            throw pybind11::stop_iteration();

        auto value = CNodeRefList<T>(&(*m_it));
        ++m_it;

        return value;
    }

private:
    typename IteratorType::const_iterator m_it;
    typename IteratorType::const_iterator const m_cend;
};

using OuterRingIterator = RingIterator<osmium::OuterRing>;
using InnerRingIterator = RingIterator<osmium::InnerRing>;

#endif //PYOSMIUM_OSM_HELPER_HPP
