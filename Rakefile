BUILD_DIR = 'build'
OBJECTS_BUILD_DIR = File.join(BUILD_DIR, 'base-objects')
EXECUTABLE_OBJECT = File.join(OBJECTS_BUILD_DIR, 'product-template.o')
PRODUCTS_DIR = File.join(BUILD_DIR, 'products')

SOURCES = %w{ source/stub.c source/string_io.c source/untar.c source/lz4/lz4.c source/lz4/lz4hc.c source/lz4/lz4io.c source/lz4/xxhash.c }
OBJECTS = SOURCES.map do |source|
  File.join(OBJECTS_BUILD_DIR, "#{File.basename(source, '.c')}.o")
end

task :objects do
  mkdir_p OBJECTS_BUILD_DIR
  SOURCES.zip(OBJECTS).each do |source, object|
    # TODO -Weverything
    command = "clang -g -O0 -Wall -c #{source} "
    command << (ENV['ANALYZE'] ? '--analyze -o /dev/null' : "-o #{object}")
    sh command
  end
end

desc 'Create stub'
task :stub => :objects do
  sh "ld -r #{OBJECTS.join(' ')} -o #{EXECUTABLE_OBJECT}"
end

desc 'Run clang analyzer'
task :analyze do
  ENV['ANALYZE'] = '1'
  Rake::Task[:objects].invoke
end

desc 'Link data file into final binary'
task :link => :stub do
  product = ENV['BINNAME'] || raise('[!] Specify BINNAME env var.')
  data_file = ENV['DATAFILE'] || raise('[!] Specify DATAFILE env var.')
  exec_cmd = ENV['EXECCMD'] || raise('[!] Specify EXECCMD env var.')

  product_object = File.join(PRODUCTS_DIR, "#{product}.o")
  ext = File.extname(data_file)[1..-1]
  mkdir_p PRODUCTS_DIR

  if ext == 'lz4'
    original_file = ENV['ORIGINALFILE'] || raise('[!] Specify ORIGINALFILE env var.')
    lz4_size_file = '/tmp/lz4-size'
    File.open(lz4_size_file, 'w') { |f| f.write(File.size?(original_file)) }
    lz4_size_command = "-sectcreate __DATA __lz4_size '#{lz4_size_file}'"
  end
  cmd_file = '/tmp/exec_cmd'
  File.open(cmd_file, 'w') { |f| f.write(exec_cmd) }

  sh "ld -r #{EXECUTABLE_OBJECT} -sectcreate __DATA __#{ext}_data '#{data_file}' -sectcreate __DATA __exec_cmd '#{cmd_file}' #{lz4_size_command} -o '#{product_object}'"
  sh "clang #{product_object} -o #{File.join(PRODUCTS_DIR, product)}"
end

desc 'Clean'
task :clean do
  rm_rf 'build'
end

desc 'Build and run'
task :run => :link do
  sh File.join(PRODUCTS_DIR, ENV['BINNAME'])
end

ENV['BINNAME'] ||= 'test'
#ENV['DATAFILE'] ||= 'fixtures/test.tar'
ENV['DATAFILE'] ||= 'fixtures/test.tar.lz4'
ENV['ORIGINALFILE'] ||= 'fixtures/test.tar'
ENV['EXECCMD'] ||= '/bin/ls -l /tmp/KISStribution.XXXXX'

task :default => :run
