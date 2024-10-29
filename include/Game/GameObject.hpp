#pragma once

#include "Bindings.h"

#include <string>

using std::string;

/**
 * @brief A generic game object, meant to be
 * inherited by more specialized objects.
 * 
 * Contains:
 * 
 * 1) A UUID, which consists of two parts:
 * 
 * a) the type - a 32-bit unsigned integer that uniquely
 * identifies this object in terms of its type
 * 
 * b) the instance - a 32-bit unsigned integer that uniquely
 * identifies which instance of the object it is
 * 
 * There can be 2^32 different kinds of objects and
 * each kind can be instantiated 2^32 times.
 * 
 * 2) 64 flags - true/false values, indexed from 0 to 63
 * 
 * This class doesn't define any use for flags, so any child
 * class is free to define its own use for them.
 * 
 * 3) A pointer to a C-style string, whose contents cannot be modified.
 * 
 * Child classes should use it to bind to a name contained in the
 * game registry, but are free to bind to whatever they want,
 * provided that they will manage the pointer.
 */
class GameObject {
    protected:
        /**
         * @brief A unique ID of the instantiated object.
         * Consists of two parts:
         * the first 32 bits are for the instance number,
         * the next 32 bits are for the type ID.
         */
        union {
            uint64_t uuid;
            char bytes[sizeof(uint64_t)];
            /**
             * @brief We're both assuming little-endian system
             * AND using what's considered undefined
             * behavior by C++ - fabulous!
             */
            struct {
                //Instance number of the object
                uint32_t instance;
                //A uniquely identifiable object type
                uint32_t type;
            } parts;
        } uuid;

        /**
         * @brief GameObject class reserves
         * 64 bits for derived subclasses
         * for any use - be it flags, bitfields
         * or any thing 64 bits wide - it's free real estate!
         */
        union {
            uint64_t all;
            char bytes[sizeof(uint64_t)];
        } flags;
        
        /**
         * @brief Pointer to a C string containing
         * the object name. Will generally bind to some registry.
         * 
         * The reason why it's not an `std::string`
         * is that every instantiated object would create a copy
         * of the name, which is inefficient, while `std::string&`
         * introduces a layer of memory indirection,
         * which is also unwanted.
         * 
         * Care shall be taken for managing object names and making
         * sure they don't jump on the heap...and if they do, then
         * every object instance bound to the now invalid pointer shall
         * be updated.
         */
        const char* name;

    protected:
        /**
         * @brief Sets the name of the object. It only sets the reference,
         * not the value, be doubly careful as `std::string`
         * can relocate in memory, invalidating the previous pointer.
         * 
         * It shouldn't be much of an issue though as object names will generally
         * be unchanged throughout execution...
         * 
         * @param name name
         */
        void setName(const string& name) { this->name = name.c_str();}

        /**
         * @brief Sets the name of the object. It only sets the reference,
         * not the value, be careful.
         * 
         * @param name name
         */
        void setName(const char* name) { this->name = name; }

        /**
         * @brief Sets the UUID of the object.
         * 
         * @param uuid UUID
         */
        void setUUID(uint64_t uuid) { this->uuid.uuid = uuid; }
        /**
         * @brief Sets the instance ID of the object.
         * 
         * @param instance instance ID
         */
        void setInstanceID(uint32_t instance) { this->uuid.parts.instance = instance; }
        /**
         * @brief Set the type ID of the object.
         * 
         * @param type type ID
         */
        void setTypeID(uint32_t type) { this->uuid.parts.type = type; }

        /**
         * @brief Sets the `flag`th flag of the object.
         * 
         * The behavior when value of `flag` is larger than 63 is undefined.
         * 
         * @param flag index from 0 to 63 of the bit to set
         */
        void setFlag(uint32_t flag) { this->flags.all |= ((uint64_t)1 << flag); }
        /**
         * @brief Sets the entire object flags to `flags`.
         * 
         * @param flags flags
         */
        void setFlags(uint64_t flags) { this->flags.all = flags; }
        /**
         * @brief Clears the `flag`th flag of the object.
         * 
         * The behavior when value of `flag` is larger than 63 is undefined.
         * 
         * @param flag index from 0 to 63 of the bit to clear
         */
        void clearFlag(uint32_t flag) { this->flags.all &= ~((uint64_t)1 << flag); }
        /**
         * @brief Clears all flags in the object.
         * 
         */
        void clearFlags() { this->flags.all = (uint64_t)0; }
        /**
         * @brief Flips the `flag`th flag of the object.
         * 
         * @param flag index from 0 to 64 of the bit to flip
         */
        void flipFlag(uint32_t flag) { this->flags.all ^= ((uint64_t)1 << flag); }

    public:
        virtual ~GameObject() = 0;

        /**
         * @brief Get the name of the object as C string.
         * 
         * @return const char* to the name
         */
        const char* getName() const { return this->name; }
        
        /**
         * @brief Get flags of the object.
         * 
         * @return copy of the flags
         */
        uint64_t getFlags() const { return this->flags.all; }
        /**
         * @brief Checks the `flag`th flag of the object.
         * 
         * The behavior when value of `flag` is larger than 63 is undefined.
         * 
         * @param flag index from 0 to 63 of the bit to check
         * @return whether the `flag`th flag is set
         */
        bool isFlagSet(uint32_t flag) const { return this->flags.all & ((uint64_t)1 << flag); }

        /**
         * @brief Get UUID of the object.
         * 
         * @return copy of UUID of the object
         */
        uint64_t getUUID() const { return this->uuid.uuid; }
        /**
         * @brief Get the instance ID of the object.
         * 
         * @return copy of instance ID of the object
         */
        uint32_t getInstanceID() const { return this->uuid.parts.instance; }
        /**
         * @brief Get the type ID of the object.
         * 
         * @return copy of type ID of the object
         */
        uint32_t getTypeID() const { return this->uuid.parts.type; }
};