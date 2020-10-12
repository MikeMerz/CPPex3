//
// Created by mikemerzl on 17/01/2020.
//

#ifndef CPPEX3_HASHMAP_HPP
#define CPPEX3_HASHMAP_HPP
//---------------DEFINES--------------
#define INITIAL_CAPACITY 16

#define STARTING_SIZE 0

#define LOWER_BOUND 0.25

#define UPPER_BOUND 0.75

#define CAPACITY_CHANGE 2

#include <vector>
#include <functional>

/**
 * Size Exception
 */
class sizeException : public std::exception
{
public:
    /**
     * Exception to be thrown when invalid size
     * @return String of exception.
     */
    const char *what () const noexcept override
    {
        return "Invalid Size";
    }
};

/**
 * Index Exception
 */
class indexException : public std::exception
{
public:
    /**
  * Exception to be thrown when invalid index
  * @return String of exception.
  */
    const char *what () const noexcept override
    {
        return "Invalid Index";
    }
};

template<typename KeyT , typename ValueT>
/**
 * Hash map of keyT and ValutT pairs
 * @tparam KeyT type of Key
 * @tparam ValueT type of Value
 */
class HashMap
{
public:
    /**
     * Def const
     */
    HashMap () : _capacity (INITIAL_CAPACITY) , _size (STARTING_SIZE) , loadFactor (LOWER_BOUND) ,
                 upFactor (UPPER_BOUND)
    {
        map = new std::vector<std::pair<KeyT , ValueT>>[_capacity];
    }

/**
 * Constcutor for tow vectors
 * @param Key Vector made of keys
 * @param Value Vector made of values
 */
    HashMap (const std::vector<KeyT> & Key , const std::vector<ValueT> & Value) : _capacity (
            INITIAL_CAPACITY) ,
                                                                                  _size (STARTING_SIZE) ,
                                                                                  loadFactor (
                                                                                          LOWER_BOUND) ,
                                                                                  upFactor (
                                                                                          UPPER_BOUND)
    {
        map = new std::vector<std::pair<KeyT , ValueT>>[_capacity];
        if (Key.size () != Value.size ())
        {
            delete[]map;
            throw sizeException ();
        }
        for (unsigned int i = 0 ; i < Key.size () ; ++ i)
        {
            insert (Key[i] , Value[i]);
        }

    }

    /**
     * Copy Constcutor
     * @param other Hashmap to copy
     */
    HashMap (const HashMap & other) : _capacity (INITIAL_CAPACITY) ,
                                      _size (STARTING_SIZE) , loadFactor (LOWER_BOUND) ,
                                      upFactor (UPPER_BOUND)
    {
        _capacity = other.capacity ();
        _size = other.size ();
        map = new std::vector<std::pair<KeyT , ValueT>>[_capacity];
        for (int i = 0 ; i < other.capacity () ; ++ i)
        {
            for (auto j = other.map[i].begin () ; j != other.map[i].end () ; ++ j)
            {
                map[i].push_back (*j);
            }
        }
    }

    /**
     * Destructor
     */
    ~HashMap ()
    {
        delete[]map;
    }

    /**
     * Inserts the given key with value
     * @param key Key to insert
     * @param value Value to insert
     * @return true if inserted false otherwise
     */
    bool insert (KeyT key , ValueT value)
    {
        if (containsKey (key))
        {
            int final = std::hash<KeyT> {} (key) & (_capacity - 1);
            for (auto i = map[final].begin () ; i != map[final].end () ; ++ i)
            {
                if ((*i).first == key)
                {
                    (*i).second = value;
                    return false;
                }
            }
            return false;
        }
        if ((_size + 1) > upFactor * _capacity)
        {
            _rehash (_capacity * CAPACITY_CHANGE);
        }
        int final = std::hash<KeyT> {} (key) & (_capacity - 1);
        map[final].push_back (std::make_pair (key , value));
        ++ _size;
        return true;

    }

    /**
     * Clears the map of items
     */
    void clear ()
    {
        for (int i = 0 ; i < _capacity ; ++ i)
        {
            _size -= map[i].size ();
            map[i].clear ();
        }
    }

    /**
     * Checks if the key given is already in the map
     * @param key Key to check
     * @return true if contains false otherwise
     */
    bool containsKey (KeyT key) const
    {
        int firstIndex = std::hash<KeyT> {} (key);
        int finalIndex = firstIndex & (_capacity - 1);
        for (auto i = map[finalIndex].begin () ; i != map[finalIndex].end () ; ++ i)
        {
            std::pair<KeyT , ValueT> temp = *i;
            if (temp.first == key)
            {
                return true;
            }
        }
        return false;

    }

    /**
 * check if the Hashmap is empty
 * @return true if empty false otherwise
 */
    bool empty () const
    {
        return _size == 0;
    }

    /**
     * Erases the key given
     * @param key Key to earse
     * @return true if erased false otherwise
     */
    bool erase (KeyT key)
    {
        if (empty ())
        {
            return false;
        }
        int index = std::hash<KeyT> {} (key) & (_capacity - 1);
        for (auto i = map[index].begin () ; i != map[index].end () ; ++ i)
        {
            if ((*i).first == key)
            {
                map[index].erase (i);
                -- _size;
                while (_capacity > 0 && getLoadFactor () < loadFactor)
                {
                    _rehash (_capacity / CAPACITY_CHANGE);
                }
                return true;
            }
        }
        return false;
    }

    /**
     * Current load factor of the map
     * @return Load factor
     */
    double getLoadFactor () const
    {
        return (double) _size / _capacity;
    }

    /**
     * Size of the bucket in the key index
     * @param key Key to check
     * @return size of the bucket
     */
    int bucketSize (KeyT key) const
    {
        int index = bucketIndex (key);
        return map[index].capacity ();
    }

    /**
     * Index the bucket for the given key
     * @param key Key to check
     * @return Index of the bucket for the given key
     */
    int bucketIndex (KeyT key) const
    {
        if (containsKey (key))
        {
            return (std::hash<KeyT> {} (key) & (_capacity - 1));
        }
        else
        {
            throw indexException {};
        }
    }

    /**
 * returns the value of the given key
 * @param key Key to return its value
 * @return ValueT of the given key
 */
    ValueT & operator[] (const KeyT & key)
    {
        try
        {
            return at (key);

        }
        catch (indexException & e)
        {
            insert (key , ValueT {});
            return at (key);
        }
    }

    /**
     * return the value of the given key if its in the map 0 otherwise
     * @param key Key whose ValueT to return
     * @return the value of the given key if its in the map 0 otherwise
     */
    ValueT & operator[] (const KeyT & key) const
    {
        try
        {
            return at (key);

        }
        catch (indexException & e)
        {
            return 0;
        }
    }

    /**
     * Returns the current capacity of the map
     * @return Capacity of the map
     */
    int capacity () const
    {
        return _capacity;
    }

    /**
     * return the number of elements in the map
     * @return number of elements in the map
     */
    int size () const
    {
        return _size;
    }

    /**
     * return the ValueT of the given key,if doesnt exist throw exception
     * @param key Key to check
     * @return ValueT if key exists indexException otherwise
     */
    ValueT & at (KeyT key) const
    {
        try
        {
            int index = bucketIndex (key);
            for (int i = 0 ; i < (int) map[index].capacity () ; ++ i)
            {
                if (map[index].at (i).first == key)
                {
                    return map[index].at (i).second;

                }
            }
            return map[index].at (0).second;
        }
        catch (indexException & e)
        {
            throw indexException {};
        }

    }

    /**
     * copies the map
     * @param other map to copy
     * @return HashMap of the copied map
     */
    HashMap & operator= (const HashMap & other)
    {
        if (this == &other)
        {
            return *this;
        }
        delete[]map;
        map = new std::vector<std::pair<KeyT , ValueT>>[other._capacity];
        _size = other._size;
        _capacity = other._capacity;
        for (int i = 0 ; i < _capacity ; ++ i)
        {
            map[i] = other.map[i];
        }

        return *this;
    }

    /**
     * Check if maps equal
     * @param other other map to check
     * @return true if equal false otherwise
     */
    bool operator== (const HashMap & other) const
    {
        if (_size != other._size)
        {
            return false;
        }
        for (int i = 0 ; i < _capacity ; ++ i)
        {

            if (map[i] != other.map[i])
            {
                return false;
            }
        }
        return true;
    }

    /**
     * Check if maps are not equal
     * @param other other map to check
     * @return true if not equal false otherwise
     */
    bool operator!= (const HashMap & other) const
    {
        if (_size != other._size)
        {
            return true;
        }
        for (int i = 0 ; i < _capacity ; ++ i)
        {
            if (map[i] != other.map[i])
            {
                return true;
            }
        }
        return false;
    }

    /**
     * Const Iterator class
     */
    class const_iterator
    {
    public:
        typedef int difference_type;
        typedef std::pair<KeyT , ValueT> value_type;
        typedef std::pair<KeyT , ValueT> *pointer;
        typedef std::pair<KeyT , ValueT> & reference;
        typedef std::forward_iterator_tag iterator_category;

        /**
         * Constcutor for iterator
         * @param now Map to represent the Hashmap
         * @param bucketNum Starting bucket position
         * @param index Starting index inside the bucket
         * @param pair Current pair
         * @param size Current amount of elements
         */
        const_iterator (std::vector<std::pair<KeyT , ValueT> > *now = nullptr , int bucketNum = 0 ,
                        int index = 0 , pointer pair = nullptr , int size = 0) : hashMap (now) ,
                                                                                 bucketNum (
                                                                                         bucketNum) ,
                                                                                 indexInBucket (
                                                                                         index) ,
                                                                                 cur (pair) ,
                                                                                 _mySize (size)
        {
        };

        /**
         * copies the ierator
         * @param constIterator other iterator to copy
         * @return New iterator
         */
        const_iterator & operator= (const const_iterator & constIterator)
        {
            if (this == &constIterator)
            {
                return *this;
            }
            cur->first = constIterator.cur->first;
            cur->second = constIterator.cur->second;
            bucketNum = constIterator.bucketNum;
            indexInBucket = constIterator.indexInBucket;
            hashMap = constIterator.hashMap;
            return *this;
        }

        /**
         * return the valueT of the current pair
         * @return valueT of the current pair
         */
        value_type operator* ()
        {
            return value_type (cur->first , cur->second);
        }

        /**
         * Pointer to the current pair
         * @return Pointer to the current pair
         */
        pointer operator-> () const
        {
            return cur;
        }

        /**
         * Advances the iterator foward
         * @return Iterator after the advance
         */
        const_iterator & operator++ ()
        {
            if (indexInBucket + 1 < hashMap[bucketNum].size ())
            {
                indexInBucket ++;
            }
            else
            {
                bucketNum ++;
                while (bucketNum < _mySize && hashMap[bucketNum].size () == 0)
                { bucketNum ++; }
                indexInBucket = 0;
                if (bucketNum > _mySize - 1)
                {
                    cur = nullptr;
                    return *this;
                }
            }

            cur = &(hashMap[bucketNum].at (indexInBucket));
            return *this;
        }

        /**
            * Advances the iterator foward
            * @return Iterator after the advance
            */
        const_iterator operator++ (int)
        {
            const_iterator tmp (*this);
            ++ *this;
            return tmp;
        }

        /**
         * Checks if iterator not equal
         * @param other Iterator to check
         * @return true if not equal false otherwise
         */
        bool operator!= (const_iterator const & other) const
        {
            return cur != other.cur;
        }

        /**
         * Checks if iterator equal
         * @param other Iterator to check
         * @return true if equal false otherwise
         */
        bool operator== (const_iterator const & other) const
        {
            return cur == other.cur;
        }

    private:
        std::vector<std::pair<KeyT , ValueT> > *hashMap;
        int bucketNum;
        unsigned int indexInBucket;
        pointer cur;
        int _mySize;
    };

    /**
     * Starting position of the iterator
     * @return Starting position of the iterator
     */
    const_iterator begin () const
    {
        int i = 0;
        while (i < this->_capacity && map[i].size () == 0)
        {
            ++ i;
        }
        if (i == capacity ())
        {
            return const_iterator (nullptr);
        }
        const_iterator temp = const_iterator (this->map , i , 0 , &map[i].at (0) , capacity ());
        return temp;
    }

    /**
      * Starting position of the const_iterator
      * @return Starting position of the iterator
      */
    const_iterator cbegin () const
    {
        int i = 0;
        while (i < capacity () && map[i].size () == 0)
        {
            ++ i;
        }
        if (i == capacity ())
        {
            return const_iterator (nullptr);
        }
        const_iterator temp = const_iterator (this->map , i , 0 , &map[i].at (0) , capacity ());
        return temp;
    }

    /**
  * End position of the iterator
  * @return End position of the iterator
  */
    const_iterator end () const
    {
        return const_iterator (nullptr);
    }

    /**
  * End position of the const_iterator
  * @return End position of the iterator
  */
    const_iterator cend ()
    {
        return const_iterator (nullptr);
    }

private:
    /**
     * current capacity
     */
    int _capacity;
    /**
     * current amount of elements
     */
    int _size;
    /**
     * load Factor for both up adn down
     */
    double loadFactor , upFactor;
    /**
     * map to represent the Hashmap
     */
    std::vector<std::pair<KeyT , ValueT> > *map;

    /**
     * Rehashes the map if needed
     * @param newCapacity New capacity after rehash
     */
    void _rehash (int newCapacity)
    {
        int oldCapacity = _capacity;
        _capacity = newCapacity;
        auto *newMap = new std::vector<std::pair<KeyT , ValueT>>[newCapacity];
        for (int i = 0 ; i < oldCapacity ; ++ i)
        {
            for (auto j = map[i].begin () ; j != map[i].end () ; ++ j)
            {
                std::pair<KeyT , ValueT> temp = *j;
                int firstIndex = std::hash<KeyT> {} (temp.first);
                int finalIndex = firstIndex & (newCapacity - 1);
                newMap[finalIndex].push_back (std::make_pair (temp.first , temp.second));
            }
        }
        delete[]map;
        map = newMap;
    }
};

#endif //CPPEX3_HASHMAP_HPP
