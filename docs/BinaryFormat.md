V2MP Binary Format
===================

A V2MP binary is a file consisting of a metadata header, followed by one or more sections of binary data. The header is specified as a UTF-8 JSON string, and contains details about the rest of the data sections present in the file.

A V2MP binary is loaded by the supervisor. The supervisor uses the contents of the file to set up and run the virtual processor. For a typical program, this will include executable code and RAM contents. The virtual processor itself is not expected to interpret the binary file as a whole.

## Overall Structure

As the binary format is flexible, a variety of data may be supplied in a V2MP binary file. Some typical formats are given below.

### Executable

An executable file contains at least one page of executable machine code for the virtual processor, and metadata in the header specifying the parameters within which to execute this code (for example, the entry point address, and the amount of stack memory to allocate). Executables will typically also include some memory initialisation data, such as values which should exist in RAM at specific addresses.

### Shared Library

A shared library file contains at least one page of executable machine code for the virtual processor, and metadata in the header specifying addresses of exported code routines. An executable program may make use of these routines, thereby allowing the code in the library to be shared among multiple programs. Libraries will typically also include some memory initialisation data, such as values which should exist in RAM at specific addresses. Which RAM pages are shared, and which are unique to each executable using the library, may be specified in the header.

## Header Format

The V2MP binary header is specified as a UTF-8 encoded, null-terminated JSON string. Data is held within a root array, where important version information is provided in a known structure in the first array index. This is to allow the JSON to be SAX-parsed, so that the supervisor may determine basic information about the binary without needing to parse the entire header.

Subsequent array indices may be used to provide data specific to the type of the binary. Each index is expected to contain a JSON object (a *descriptor object*), and each descriptor object is expected to contain at least a `"type"` property which indicates purpose of the data specified. A descriptor object missing any required properties, or an array entry which is not a JSON object, will cause the supervisor to abort the loading of the binary file. Any superfluous descriptor object properties, or descriptor objects whose types are not recognised, are ignored by the supervisor when loading the binary file.

The only currently supported binary header version number is `1`. Different object types that may be present in a version `1` header are described below.

### Version Descriptor Object

For any valid V2MP binary header, the first item in the JSON array **must** be the version object. This is a descriptor object with two properties, both of which are required.

| Key | Value Type | Expected Value | Description |
|-----|------------|----------------|-------------|
| `identifier` | String | `"v2mpbinf"` | Identifier string for this format of binary file. |
| `version` | Number | `1` | Version of the metadata specified in the header. |

If the `identifier` is not `"v2mpbinf"`, or the version number is not `1`, the file will not be loaded.

### Executable Descriptor Object

If the binary file represents an executable, the second item in the header's JSON array **must** be an executable descriptor object. This is a descriptor object with the following properties:

| Key | Value Type | Default If Not Specified | Description |
|-----|------------|--------------------------|-------------|
| `type` | String | (required) | Type of this descriptor object: must be `"executable"`. |
| `code_pages` | Array of [code page descriptor objects](#code-page-descriptor-object) | (required) | Array of objects defining the internal code pages usable by the executable. At least one code page must be present in this array. |
| `data_pages` | Array of [data page descriptor objects](#data-page-descriptor-object) | `[]` | Array of objects defining the internal data pages usable by the executable. |
| `entry_point` | [Entry point descriptor object](#entry-point-descriptor-object) | (required) | Object defining the entry point (code page and address) for the executable. This is where execution begins when the executable is loaded. |
| `stack_size_bytes` | Number | `0` | Stack size that the supervisor should allocate for use by the executable. This is specified in bytes, but is expected to be a multiple of the size of a word. |

### Shared Library Descriptor Object

If the binary file represents a shared library, the second item in the header's JSON array **must** be a shared library descriptor object. This is a descriptor object with the following properties:

| Key | Value Type | Default If Not Specified | Description |
|-----|------------|--------------------------|-------------|
| `type` | String | (required) | Type of this descriptor object: must be `"shared_library"`. |
| `code_pages` | Array of [code page descriptor objects](#code-page-descriptor-object) | (required) | Array of objects defining the internal code pages usable by the shared library. At least one code page must be present in this array. |
| `data_pages` | Array of [data page descriptor objects](#data-page-descriptor-object) | `[]` | Array of objects defining the internal data pages usable by the shared library. |
| `exported_symbols` | Array of [exported symbol descriptor objects](#exported-symbol-descriptor-object)| (required) | Array of objects defining the addresses of different exported symbols in the shared library. If any user of the library requests a symbol that is not exported here, an error is raised at runtime. (TODO: What error?) |

### Code Page Descriptor Object

A code page descriptor object describes a section of the binary file which corresponds to executable machine code.

| Key | Value Type | Default If Not Specified | Description |
|-----|------------|--------------------------|-------------|
| `type` | String | (required) | Type of this descriptor object: must be `"code_page"`. |
| `index` | Number | (required) | Index of this code page. This is expected to be unique among all code page descriptor objects. |
| `begin_file_offset_bytes` | Number | (required) | Offset in the file at which the code page's data may be found. This is an offset in bytes from the beginning of the binary file itself. |
| `page_size_bytes` | Number | (required) | Size of the code page. This is specified in bytes, but the value expected to be a multiple of the size of a word. A section of this size is loaded by the supervisor from the binary file from the offset given by `begin_file_offset_bytes`. |

### Data Page Descriptor Object

A data page descriptor object describes a section of the binary file which corresponds to usable data. The data page is loaded into RAM and accessed by the executable or shared library.

A data page has a pre-determined size, and may or may not have initialisation data specified as part of the binary file. Any initialisation data is loaded into RAM when the data page is first loaded, and is written into the page beginning at address `0x0000`. Any other data is left uninitialised.

| Key | Value Type | Default If Not Specified | Description |
|-----|------------|--------------------------|-------------|
| `type` | String | (required) | Type of this descriptor object: must be `"data_page"`. |
| `index` | Number | (required) | Index of this data page. This is expected to be unique among all data page descriptor objects. |
| `page_size_bytes` | Number | (required) | Size of the data page. This is specified in bytes. This size corresponds to the size of the data page as a whole - the number of bytes of initialisation data provided in the binary file may be smaller than this, but may not be larger. |
| `init_data_file_offset_bytes` | Number | `0` | Offset in the file at which the data page's initialisation data may be found. This is an offset in bytes from the beginning of the binary file itself. If there is no initialisation data supplied for this page, this property **must** be omitted. |
| `init_data_size_bytes` | Number | `0` | Number of bytes of initialisation data provided in the binary file. These bytes are loaded by the supervisor from the binary file from the offset given by `init_data_file_offset_bytes`. If there is no initialisation data supplied for this page, this property **must** be omitted. |

### Entry Point Descriptor Object

An entry point descriptor object describes the code entry point for an executable. The program begins executing from this entry point.

| Key | Value Type | Default If Not Specified | Description |
|-----|------------|--------------------------|-------------|
| `type` | String | (required) | Type of this descriptor object: must be `"entry_point"`. |
| `code_page_index` | Number | (required) | Index of the code page containing the `code_address` where execution starts. This is expected to refer to the index of one of the code pages defined in the binary file. |
| `data_page_index` | Number | (required) | Index of the data page that should be loaded alongside the initial code page. This is expected to refer to the index of one of the data pages defined in the binary file.
| `code_address` | Number | (required) | Address within the specified code page where the first executable instruction resides. Code execution begins from this address. This address is expected to be aligned to the size of a word. |

### Exported Symbol Descriptor Object

An exported symbol descriptor object describes a symbol that is available to be referenced by a user of a shared library.

| Key | Value Type | Default If Not Specified | Description |
|-----|------------|--------------------------|-------------|
| `type` | String | (required) | Type of this descriptor object: must be `"exported_symbol"`. |
| `symbol_name` | String | (required) | Name of the symbol. This is expected to be unique among all exported symbol descriptor objects. |
| `symbol_type` | String | (required) | Type of the symbol. This may be either `"code_address"`, or `"data_buffer"`.
| `symbol_page_index` | Number | (required) | Index of the code page (if `symbol_type` is `"code_address"`), or data page (if `symbol_type` is `"data_buffer"`), where the symbol can be found. This is expected to refer to the index of one of the respective code or data pages defined in the binary file. |
| `symbol_address` | Number | (required) | Address within the specified page at which the symbol can be found. |
| `symbol_data_length_bytes` | Number | `0` | Length of the symbol's data in bytes, if `symbol_type` is `"data_buffer"`. If the length of the data exceeds the available space in the data page after the symbol's address, an error is raised at runtime. (TODO: What error?) If `symbol_type` is `"code_address"`, this property **must** be omitted. |
