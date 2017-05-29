//
// Copyright (c) 2013-2017 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BEAST_HTTP_WRITE_HPP
#define BEAST_HTTP_WRITE_HPP

#include <beast/config.hpp>
#include <beast/core/buffer_cat.hpp>
#include <beast/core/consuming_buffers.hpp>
#include <beast/core/multi_buffer.hpp>
#include <beast/http/message.hpp>
#include <beast/http/serializer.hpp>
#include <beast/http/detail/chunk_encode.hpp>
#include <beast/core/error.hpp>
#include <beast/core/async_result.hpp>
#include <beast/core/string_view.hpp>
#include <boost/variant.hpp>
#include <limits>
#include <memory>
#include <ostream>
#include <type_traits>
#include <utility>

namespace beast {
namespace http {

/** Write some serialized message data to a stream.

    This function is used to write serialized message data to the
    stream. The function call will block until one of the following
    conditions is true:
        
    @li One or more bytes have been transferred.

    @li An error occurs on the stream.

    In order to completely serialize a message, this function
    should be called until `sr.is_done()` returns `true`.
    
    @param stream The stream to write to. This type must
    satisfy the requirements of @b SyncWriteStream.

    @param sr The serializer to use.

    @throws system_error Thrown on failure.

    @see @ref async_write_some, @ref serializer
*/
template<class SyncWriteStream,
    bool isRequest, class Body, class Fields,
        class Decorator, class Allocator>
void
write_some(SyncWriteStream& stream, serializer<
    isRequest, Body, Fields, Decorator, Allocator>& sr);


/** Write some serialized message data to a stream.

    This function is used to write serialized message data to the
    stream. The function call will block until one of the following
    conditions is true:
        
    @li One or more bytes have been transferred.

    @li An error occurs on the stream.

    In order to completely serialize a message, this function
    should be called until `sr.is_done()` returns `true`.
    
    @param stream The stream to write to. This type must
    satisfy the requirements of @b SyncWriteStream.

    @param sr The serializer to use.

    @param ec Set to indicate what error occurred, if any.

    @see @ref async_write_some, @ref serializer
*/
template<class SyncWriteStream,
    bool isRequest, class Body, class Fields,
        class Decorator, class Allocator>
void
write_some(SyncWriteStream& stream, serializer<
    isRequest, Body, Fields, Decorator, Allocator>& sr,
        error_code& ec);

/** Start an asynchronous write of some serialized message data to a stream.

    This function is used to asynchronously write serialized
    message data to the stream. The function call always returns
    immediately. The asynchronous operation will continue until
    one of the following conditions is true:

    @li One or more bytes have been transferred.

    @li An error occurs on the stream.

    In order to completely serialize a message, this function
    should be called until `sr.is_done()` returns `true`.

    @param stream The stream to write to. This type must
    satisfy the requirements of @b SyncWriteStream.

    @param sr The serializer to use for writing.

    @param handler The handler to be called when the request
    completes. Copies will be made of the handler as required. The
    equivalent function signature of the handler must be:
    @code void handler(
        error_code const& ec    // Result of operation
    ); @endcode
    Regardless of whether the asynchronous operation completes
    immediately or not, the handler will not be invoked from within
    this function. Invocation of the handler will be performed in a
    manner equivalent to using `boost::asio::io_service::post`.

    @see @ref write_some, @ref serializer
*/
template<class AsyncWriteStream,
    bool isRequest, class Body, class Fields,
        class Decorator, class Allocator, class WriteHandler>
#if BEAST_DOXYGEN
    void_or_deduced
#else
async_return_type<WriteHandler, void(error_code)>
#endif
async_write_some(AsyncWriteStream& stream, serializer<
    isRequest, Body, Fields, Decorator, Allocator>& sr,
        WriteHandler&& handler);

/** Write a HTTP/1 message to a stream.

    This function is used to write a message to a stream. The call
    will block until one of the following conditions is true:

    @li The entire message is written.

    @li An error occurs.

    This operation is implemented in terms of one or more calls
    to the stream's `write_some` function.

    The implementation will automatically perform chunk encoding if
    the contents of the message indicate that chunk encoding is required.
    If the semantics of the message indicate that the connection should
    be closed after the message is sent, the error thrown from this
    function will be `boost::asio::error::eof`.

    @param stream The stream to which the data is to be written.
    The type must support the @b SyncWriteStream concept.

    @param msg The message to write.

    @throws system_error Thrown on failure.
*/
template<class SyncWriteStream,
    bool isRequest, class Body, class Fields>
void
write(SyncWriteStream& stream,
    message<isRequest, Body, Fields> const& msg);

/** Write a HTTP/1 message on a stream.

    This function is used to write a message to a stream. The call
    will block until one of the following conditions is true:

    @li The entire message is written.

    @li An error occurs.

    This operation is implemented in terms of one or more calls
    to the stream's `write_some` function.

    The implementation will automatically perform chunk encoding if
    the contents of the message indicate that chunk encoding is required.
    If the semantics of the message indicate that the connection should
    be closed after the message is sent, the error returned from this
    function will be `boost::asio::error::eof`.

    @param stream The stream to which the data is to be written.
    The type must support the @b SyncWriteStream concept.

    @param msg The message to write.

    @param ec Set to the error, if any occurred.
*/
template<class SyncWriteStream,
    bool isRequest, class Body, class Fields>
void
write(SyncWriteStream& stream,
    message<isRequest, Body, Fields> const& msg,
        error_code& ec);

/** Write a HTTP/1 message asynchronously to a stream.

    This function is used to asynchronously write a message to
    a stream. The function call always returns immediately. The
    asynchronous operation will continue until one of the following
    conditions is true:

    @li The entire message is written.

    @li An error occurs.

    This operation is implemented in terms of one or more calls to
    the stream's `async_write_some` functions, and is known as a
    <em>composed operation</em>. The program must ensure that the
    stream performs no other write operations until this operation
    completes.

    The implementation will automatically perform chunk encoding if
    the contents of the message indicate that chunk encoding is required.
    If the semantics of the message indicate that the connection should
    be closed after the message is sent, the operation will complete with
    the error set to `boost::asio::error::eof`.

    @param stream The stream to which the data is to be written.
    The type must support the @b AsyncWriteStream concept.

    @param msg The message to write. The object must remain valid
    at least until the completion handler is called; ownership is
    not transferred.

    @param handler The handler to be called when the operation
    completes. Copies will be made of the handler as required.
    The equivalent function signature of the handler must be:
    @code void handler(
        error_code const& error // result of operation
    ); @endcode
    Regardless of whether the asynchronous operation completes
    immediately or not, the handler will not be invoked from within
    this function. Invocation of the handler will be performed in a
    manner equivalent to using `boost::asio::io_service::post`.
*/
template<class AsyncWriteStream,
    bool isRequest, class Body, class Fields,
        class WriteHandler>
async_return_type<
    WriteHandler, void(error_code)>
async_write(AsyncWriteStream& stream,
    message<isRequest, Body, Fields> const& msg,
        WriteHandler&& handler);

//------------------------------------------------------------------------------

/** Serialize a HTTP/1 header to a `std::ostream`.

    The function converts the header to its HTTP/1 serialized
    representation and stores the result in the output stream.

    @param os The output stream to write to.

    @param msg The message fields to write.
*/
template<bool isRequest, class Fields>
std::ostream&
operator<<(std::ostream& os,
    header<isRequest, Fields> const& msg);

/** Serialize a HTTP/1 message to a `std::ostream`.

    The function converts the message to its HTTP/1 serialized
    representation and stores the result in the output stream.

    The implementation will automatically perform chunk encoding if
    the contents of the message indicate that chunk encoding is required.

    @param os The output stream to write to.

    @param msg The message to write.
*/
template<bool isRequest, class Body, class Fields>
std::ostream&
operator<<(std::ostream& os,
    message<isRequest, Body, Fields> const& msg);

} // http
} // beast

#include <beast/http/impl/write.ipp>

#endif
