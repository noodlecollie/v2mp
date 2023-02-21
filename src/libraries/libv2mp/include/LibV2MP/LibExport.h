#ifndef V2MP_LIBEXPORT_H
#define V2MP_LIBEXPORT_H

// Defined if building shared libraries.
/* #undef V2MP_IS_SHARED_BUILD */

#if defined(V2MP_IS_SHARED_BUILD) && (defined(WIN32) || defined(WIN64))

#if defined(V2MP_PRODUCER)
// This build is producing a library for another application to use.
// Mark symbols as being exported.
#define API_LIBV2MP __declspec(dllexport)
#else // defined(V2MP_PRODUCER)
// This library is being used by some application.
// Mark symbols as being imported.
#define API_LIBV2MP __declspec(dllimport)
#endif // defined(V2MP_PRODUCER)

#else // defined(V2MP_IS_SHARED_BUILD) && (defined(WIN32) || defined(WIN64))

// We're not using shared libraries, or we're not building under Windows.
// No need to use __declspec, so define this to nothing.
#define API_LIBV2MP

#endif // defined(V2MP_IS_SHARED_BUILD) && (defined(WIN32) || defined(WIN64))
#endif // V2MP_LIBEXPORT_H
