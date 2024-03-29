# This file is part of the rbh-find-iosea project
# Copyright (C) 2024 Commissariat a l'energie atomique et aux energies
# 		      alternatives
#
# SPDX-License-Identifer: LGPL-3.0-or-later

project(
	'rbh-find-iosea',
	'c',
	version: '0.0.0',
	license: 'LGPL3.0-or-later',
	default_options: [
		'warning_level=3',
		'werror=true',
	],
)

# Project extentions
add_project_arguments('-D_GNU_SOURCE', language: 'c')

# Configuration checks
conf_data = configuration_data()

## Functions
cc = meson.get_compiler('c')
have_statx = cc.has_function('statx', args: '-D_GNU_SOURCE',
			     prefix: '#include <sys/stat.h>')
conf_data.set('HAVE_STATX', have_statx)

configure_file(input: 'config.h.in', output: 'config.h',
	       configuration: conf_data)
add_project_arguments(['-DHAVE_CONFIG_H',], language: 'c')

librobinhood = dependency('robinhood', version: '>=0.0.0')
librbhfind = dependency('rbh-find', version: '>=0.0.0')

# "." is necessary for config.h
include_dirs = include_directories('.', 'include')

executable(
	'rbh-ifind',
	sources: [
		'rbh-find-iosea.c',
		'src/filters.c',
		'src/parser.c',
		],
	dependencies: [librobinhood, librbhfind],
	include_directories: include_dirs,
	install: true,
)
subdir('tests')
