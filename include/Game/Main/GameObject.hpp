#pragma once

#include "Bindings.h"

#include <string>

#include "Game/Main/MainRegistry.hpp"

using std::string;

extern const char* defaultObjectName;

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
 * This class reserves flag 0 to mark the object as scheduled
 * for deletion. Child classes shall not use this flag as it may
 * cause a memory leak.
 * 
 * 3) A pointer to a C-style string, whose contents cannot be
 * modified directly.
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
        UUID uuid;

        /**
         * @brief GameObject class reserves
         * 64 bits for derived subclasses
         * for any use - be it flags, bitfields
         * or any thing 64 bits wide - it's free real estate!
         */
        //TODO: make `all` atomic for future multithreaded access...maybe, idk
        union {
            uint64_t all;
            char bytes[sizeof(uint64_t)];
        } flags;
        
        /**
         * @brief Pointer to a C string containing
         * the object name. Will generally bind to
         * a read-only memory segment (a string literal)
         * or an override in the registry. Defaults to an
         * empty C string ("").
         * 
         * The reason why it's not an `std::string`
         * is that every instantiated object would create a copy
         * of the name potentially on the heap, which by itself
         * is inefficient, but also takes 32 bytes of memory,
         * while `std::string&`
         * introduces a layer of memory indirection,
         * which is also unwanted.
         * 
         * Care shall be taken for managing object names and making
         * sure they don't jump on the heap...and if they do, then
         * every object instance bound to the now invalid pointer shall
         * be updated.
         */
        const char* name = defaultObjectName;
    protected:
        /**
         * @brief Sets the name of the object.
         * 
         * If you want the name to be modifiable at runtime, you
         * have to use an externally modifiable `char*`, possibly from
         * the game's registry,
         * not a literal (literals are read-only).
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
         * @brief Clears all flags in the object. Generally discouraged.
         */
        void clearFlags() { this->flags.all = (uint64_t)0; }
        /**
         * @brief Flips the `flag`th flag of the object.
         * 
         * @param flag index from 0 to 64 of the bit to flip
         */
        void flipFlag(uint32_t flag) { this->flags.all ^= ((uint64_t)1 << flag); }
        /**
         * @brief Marks the object for deletion. The behavior of using
         * the object after marking it deleted is undefined. Don't do that :)
         */
        void markForDeletion() { this->setFlag(0); }
    public:
        /**
         * @brief Constructor for a GameObject.
         * Doesn't set a name,
         * which is rather difficult to change after initialization.
         * @param objectID ID of the object
         */
        GameObject(const uint32_t objectID) { this->uuid.parts.type = objectID; }
        /**
         * @brief Constructor for a GameObject.
         * 
         * @param objectID ID of the object
         * @param name name of the object
         */
        GameObject(const uint32_t objectID, const char* name) : name(name) { this->uuid.parts.type = objectID; }
        
        ~GameObject() = default;

        /**
         * @brief Get the name of the object as C string.
         * 
         * @return const char* to the name
         */
        const char* getName() const { return this->name; }
        
        /**
         * @brief Get flags of the object.
         * 
         * @return copy of the flags as a 64-bit integer
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