project('etceteras', 'cpp',
    license: 'MIT',
    default_options: [
        'cpp_std=c++17',
        'warning_level=3'])

headers = [
    'include/etceteras/expected.hpp'
]

incdirs = include_directories('./include')

etceteras = declare_dependency(
    include_directories: incdirs,
    sources: headers
)

# subdir('benchmark')
subdir('test')

install_headers(headers, subdir: 'etceteras')

pkg = import('pkgconfig')
pkg.generate(
    name: 'etceteras',
    filebase: 'etceteras',
    subdirs: ['etceteras'],
    description: 'Kind of std::expected, etc.'
)
