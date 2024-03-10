/* SPDX-License-Identifier: BSD-2-Clause
 *
 * This file is part of pyosmium. (https://osmcode.org/pyosmium/)
 *
 * Copyright (C) 2024 Sarah Hoffmann <lonvia@denofr.de> and others.
 * For a full list of authors see the git log.
 */
#ifndef PYOSMIUM_HANDLER_CHAIN_H
#define PYOSMIUM_HANDLER_CHAIN_H

#include <vector>

#include <pybind11/pybind11.h>

#include <osmium/handler.hpp>

#include "base_handler.h"
#include "python_handler.h"

namespace py = pybind11;

class HandlerChain : public osmium::handler::Handler
{
public:
    HandlerChain(py::args args)
    {
        m_handlers.reserve(args.size());
        m_python_handlers.reserve(args.size());
        for (auto &arg: args) {
            if (py::isinstance<BaseHandler>(arg)) {
                // Already a handler object, push back directly.
                m_handlers.push_back(arg.cast<BaseHandler *>());
            } else if (py::hasattr(arg, "node") || py::hasattr(arg, "way")
                       || py::hasattr(arg, "relation")
                       || py::hasattr(arg, "changeset") || py::hasattr(arg, "area")) {
                // Python object that looks like a handler.
                // Wrap into a osmium handler object.
                m_python_handlers.emplace_back(arg);
                m_handlers.push_back(&m_python_handlers.back());
            } else {
                throw py::type_error{"Argument must be a handler-like object."};
            }
        }
    }

    void node(osmium::Node const &o) {
        for (auto const &handler : m_handlers) {
            if (handler->node(&o)) {
                return;
            }
        }
    }

    void way(osmium::Way &w) {
        for (auto const &handler : m_handlers) {
            if (handler->way(&w)) {
                return;
            }
        }
    }

    void relation(osmium::Relation const &o) {
        for (auto const &handler : m_handlers) {
            if (handler->relation(&o)) {
                return;
            }
        }
    }

    void changeset(osmium::Changeset const &o) {
        for (auto const &handler : m_handlers) {
            if (handler->changeset(&o)) {
                return;
            }
        }
    }

    void area(osmium::Area const &o) {
        for (auto const &handler : m_handlers) {
            if (handler->area(&o)) {
                return;
            }
        }
    }

    void flush() {
        for (auto const &handler : m_handlers) {
            handler->flush();
        }
    }

private:
    std::vector<BaseHandler *> m_handlers;
    std::vector<pyosmium::PythonHandler> m_python_handlers;
};

#endif //PYOSMIUM_HANDLER_CHAIN_H
