#ifndef BASEUTIL_FILESYSTEM_H
#define BASEUTIL_FILESYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

// Max path lengths don't seem to be very consistent across systems.
// We just choose a length that should be long enough for any sensible path.
#define BASEUTIL_PATH_MAX 2048

#ifdef __cplusplus
} // extern "C"
#endif

#endif // BASEUTIL_FILESYSTEM_H
