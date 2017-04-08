#pragma once

struct FLIF_INFO
{
    FLIF_INFO();

    uint32_t width;
    uint32_t height;
    uint8_t channels;
    uint8_t bit_depth;
    size_t num_images;
};

/*!
* @param[out] info An info struct to fill. If this is not a null pointer, the decoding will exit after reading the file header.
*/

template <typename IO>
bool flif_decode(IO& io, Images &images, uint32_t (*callback)(int32_t,int64_t), int, Images &partial_images, flif_options &options, metadata_options &md, FLIF_INFO* info);

template <typename IO>
bool flif_decode(IO& io, Images &images, flif_options &options, metadata_options &md) {
    return flif_decode(io, images, NULL, 0, images, options, md, NULL);
}

// With callback
template <typename IO>
bool flif_decode(IO& io, Images &images, uint32_t (*callback)(int32_t,int64_t), int initialProgressThreshold, Images &partial_images, flif_options &options, metadata_options &md) {
    return flif_decode(io, images, callback, initialProgressThreshold, partial_images, options, md, NULL);
}
