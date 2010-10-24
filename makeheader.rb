#!/usr/bin/env ruby

CLASS_MAPPING = {}
[ 8, 16     ].each { |x| CLASS_MAPPING["_SFR_IO#{x}"]  = "IO#{x}" }
[ 8, 16, 32 ].each { |x| CLASS_MAPPING["_SFR_MEM#{x}"] = "Mem#{x}" }

puts <<EOF
// AUTO GENERATED HEADER FILE

// Prevent header being included twice
#if defined(CPP_PRELUDE_INCLUDED_CPU_HEADER)
#  error Already included header. Please include only cppprelude.h
#endif

#define CPP_PRELUDE_INCLUDED_CPU_HEADER 1

// Prevent header being used without cppprelude.h
#if !defined(CPP_PRELUDE_h)
#  error Missing cppprelude.h. Please include only cppprelude.h
#endif

EOF

while line = gets
  if line.match %r{^#define ([A-Z0-9]+)\s+(_SFR_(?:IO(?:8|16)|MEM(?:8|16|32)))\((0x[0-9A-F]+)\)}
    name, macro, address = $1, $2, $3

    type = name + "_t"
    classtype = CLASS_MAPPING[macro]

    puts "#undef #{name}"
    puts "typedef #{classtype}<#{address}> #{type};"
    puts "extern #{type} #{name};"
    puts
  end
end

