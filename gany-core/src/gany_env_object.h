/*
 * Copyright (c) 2022 Gxin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef GX_GANY_ENV_OBJECT_H
#define GX_GANY_ENV_OBJECT_H

#include "gx/gany.h"

#include <gx/gmutex.h>


GX_NS_BEGIN

class GAnyEnvObject : public GAnyValueP<std::unordered_map<std::string, GAny> >
{
public:
    GAnyEnvObject()
            : GAnyValueP<std::unordered_map<std::string, GAny>>({})
    {
    }

public:
    const void *as(const TypeID &tp) const override
    {
        if (GAnyTypeInfo::EqualType(tp, typeid(GAnyEnvObject))) {
            return this;
        }
        return nullptr;
    }

    GAnyClass *classObject() const override
    {
        static GAnyClass *clazz = nullptr;
        return clazz ? clazz : clazz = GAnyClass::instance<GAnyEnvObject>().get();
    }

    GAny get(const std::string &key)
    {
        GLockerGuard locker(lock);
        auto it = var.find(key);
        if (it == var.end()) {
            return GAny::undefined();
        }
        return it->second;
    }

    void set(const std::string &key, const GAny &value)
    {
        GLockerGuard locker(lock);
        auto it = var.find(key);
        if (it == var.end()) {
            var.insert(std::make_pair(key, value));
        } else {
            it->second = value;
        }
    }

    void erase(const std::string &key)
    {
        GLockerGuard locker(lock);
        auto it = var.find(key);
        if (it != var.end()) {
            var.erase(it);
        }
    }

    bool contains(const std::string &key) const
    {
        GLockerGuard locker(lock);
        return var.find(key) != var.end();
    }

public:
    mutable GMutex lock;
};

extern GAny &getEnv();

extern GAnyEnvObject &getEnvObject();

GX_NS_END

#endif //GX_GANY_ENV_OBJECT_H
