#ifndef ABSTRACT_SERIAL_PORT_COMMUNICATIONS_CLASS_H___
#define ABSTRACT_SERIAL_PORT_COMMUNICATIONS_CLASS_H___

#include <cstddef>
#include <cstdint>
#include <string>

namespace RS_232{
    class Serial_Port{
        public:
        //Types and aliases
            using count_type    = unsigned short;
            using byte_type     = std::int_least8_t;
            using size_type     = std::uint_fast16_t;
            using str_type      = std::basic_string<byte_type>;
            using vol_str_type  = std::string;

            class error_type{
                public:
                    enum class code : std::int_least8_t{
                        read, write, open, close, unavailable,
                        overflow, hardware,
                        unknown, os_specific,
                        none
                    };

                    code get_code()const
                        {return m_code;}

                    virtual const vol_str_type& what()const
                        {return m_msg;}

                    error_type()
                        : m_code(code::none)
                        , m_msg("No error.")
                    {}
                    error_type(code c, const vol_str_type& s)
                        : m_code(c)
                        , m_msg(s)
                    {}
                    error_type(const error_type&)               = default;
                    error_type(error_type&&)                    = default;
                    error_type& operator=(const error_type&)    = default;
                    error_type& operator=(error_type&&)         = default;
                    virtual ~error_type(){}
                private:
                    code            m_code;
                    vol_str_type    m_msg;
            };

        //Use standard baud rates of a weakly-typed enumeration
        //  to allow casting to primitive types
            enum baud_rate : std::int_least32_t{
                br_110 = 110,
                br_300 = 300,
                br_600 = 600,
                br_1200 = 1200,
                br_2400 = 2400,
                br_4800 = 4800,
                br_9600 = 9600,
                br_14400 = 14400,
                br_19200 = 19200,
                br_28800 = 28800,
                br_38400 = 38400,
                br_56000 = 56000,
                br_57600 = 57600,
                br_115200 = 115200
            };

        //Read-only
            bool good()
                {return m_error.get_code() == error_type::code::none;}
            bool fail()
                {return !(this->good());}
            bool is_connected()const
                {return m_connected;}
            count_type number()const
                {return m_port;}
            const error_type& error()const
                {return m_error;}
            baud_rate baud()const
                {return m_baud_rate;}
            size_type read_rate()const
                {return m_read_rate;}

        //Settings modifiers
            void set_baud_rate(baud_rate new_rate)
                {m_baud_rate = new_rate;}
            void set_read_rate(size_type bytes_to_read)
                {m_read_rate = bytes_to_read;}
            void clear_error()
                {m_error = error_type();}
            virtual const error_type& check_status() = 0;

        //Byte by byte I/O
            virtual bool open(count_type, baud_rate, size_type = 0) = 0;
            virtual bool close() = 0;
            virtual bool change(count_type, baud_rate, size_type = 0) = 0;
            virtual bool write(byte_type) = 0;
            virtual bool read(byte_type&) = 0;
            virtual bool write(
                const byte_type*,
                size_type = 0,
                size_type* = nullptr //Optional request to get number of
                                    // bytes written
            ) = 0;
            virtual bool read(
                byte_type*,
                size_type = 0,
                size_type* = nullptr //Optional request to get number of
                                    // bytes read
            ) = 0;
            virtual Serial_Port& getline(
                byte_type* buf,
                size_type num_to_read,
                byte_type delim = '\0'
            ) = 0;
            virtual Serial_Port& ignore(
                size_type num_to_ignore,
                byte_type delim = '\0'
            ) = 0;

        //Stream I/O
            virtual Serial_Port& operator<<(byte_type) = 0;
            virtual Serial_Port& operator>>(byte_type&) = 0;
            virtual Serial_Port& operator<<(str_type) = 0;
            virtual Serial_Port& operator>>(str_type&) = 0;

        //Other modifiers
            //Returns if flushing is successful or not
            virtual bool flush(
                bool output = true,
                bool force_abort = false    //Terminates read or write
                                            //  operations even if they
                                            //  have not been completed.
            ) = 0;
            virtual bool flush_input(bool force_abort = false) = 0;
            virtual bool flush_output(bool force_abort = false) = 0;

        //Constructors and destructor
            Serial_Port()
                : Serial_Port(-1, br_9600, 0)
            {}
            Serial_Port(
                count_type port_number,
                baud_rate new_baud_rate = br_9600,
                size_type new_read_rate = 0
            )
                : m_connected(false)
                , m_port(port_number)
                , m_baud_rate(new_baud_rate)
                , m_read_rate(new_read_rate)
                , m_error()
            {}
            Serial_Port(const Serial_Port&)             = default;
            Serial_Port(Serial_Port&&)                  = default;
            Serial_Port& operator=(const Serial_Port&)  = default;
            Serial_Port& operator=(Serial_Port&&)       = default;
            virtual ~Serial_Port(){}

        protected:
            bool        m_connected;
            count_type  m_port;
            baud_rate   m_baud_rate;
            size_type   m_read_rate;
            error_type  m_error;
    };

    Serial_Port& getline(
        Serial_Port&, Serial_Port::vol_str_type&,
        Serial_Port::byte_type delim = '\0'
    );
}

#endif
