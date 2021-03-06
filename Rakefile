BUILD_DIR = 'build'
OBJECTS_BUILD_DIR = File.join(BUILD_DIR, 'base-objects')
EXECUTABLE_OBJECT = File.join(OBJECTS_BUILD_DIR, 'product-template.o')
PRODUCTS_DIR = File.join(BUILD_DIR, 'products')
FINAL_PRODUCT = File.join(PRODUCTS_DIR, 'kisstribute')

SOURCES = %w{ source/kisstribute.c source/archive_data.c source/stub.c source/string_io.c source/untar.c source/lz4/lz4.c source/lz4/lz4hc.c source/lz4/lz4io.c source/lz4/xxhash.c }
OBJECTS = SOURCES.map do |source|
  File.join(OBJECTS_BUILD_DIR, "#{File.basename(source, '.c')}.o")
end

require 'tempfile'

def tmp(content)
  file = Tempfile.new('kisstribute-tempfile')
  file.write(content)
  file.close
  file.path
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
  objects = OBJECTS.reject { |f| File.basename(f) == 'kisstribute.o' }
  sh "ld -r #{objects.join(' ')} -o #{EXECUTABLE_OBJECT}"
end

desc 'Create kisstribute'
task :kisstribute => :stub do
  # TODO compressed data doesn't actually work atm
  product_template = "#{EXECUTABLE_OBJECT}.tar"
  sh "cd #{OBJECTS_BUILD_DIR} && tar -cf #{File.basename(product_template)} #{File.basename(EXECUTABLE_OBJECT)}"
  #product_template = "#{EXECUTABLE_OBJECT}.tar.lz4"
  #cmd = "tar -cf - #{EXECUTABLE_OBJECT} | lz4 -zf9 - #{product_template} 2>&1"
  #puts cmd
  #uncompressed_size = `#{cmd}`.split(/\r|\n/).last.match(/Compressed (\d+) bytes/)[1]

  object = File.join(OBJECTS_BUILD_DIR, 'kisstribute.o')
  product_object = File.join(OBJECTS_BUILD_DIR, 'kisstribute+product-template.o')

  # TODO use -Xlinker arg to clang?
  #size_file = '/tmp/lz4-size'
  #File.open(size_file, 'w') { |f| f.write(uncompressed_size) }
  #sh "ld -r #{object} -sectcreate __DATA __kiss_data #{product_template} -sectcreate __DATA __kiss_size #{size_file} -o #{product_object}"
  sh "ld -r #{object} -sectcreate __DATA __kiss_data #{product_template} -o #{product_object}"
  mkdir_p PRODUCTS_DIR
  objects = [product_object]
  objects.concat(OBJECTS.reject { |o| %w{ kisstribute.o stub.o }.include?(File.basename(o)) })
  sh "clang #{objects.join(' ')} -o #{FINAL_PRODUCT}"
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
  uid = ENV['UID'] || raise('[!] Specify UID env var.')

  product_object = File.join(PRODUCTS_DIR, "#{File.basename(product)}.o")
  ext = File.extname(data_file)[1..-1]
  mkdir_p PRODUCTS_DIR

  if ext == 'lz4'
    puts "lz4 support not working yet"
    exit 1
    #original_file = ENV['ORIGINALFILE'] || raise('[!] Specify ORIGINALFILE env var.')
    #lz4_size_file = '/tmp/lz4-size'
    #File.open(lz4_size_file, 'w') { |f| f.write(File.size?(original_file)) }
    #lz4_size_command = "-sectcreate __DATA __lz4_size '#{lz4_size_file}'"
  end
  #sh "ld -r #{EXECUTABLE_OBJECT} -sectcreate __DATA __#{ext}_data '#{data_file}' -sectcreate __DATA __exec_cmd '#{cmd_file}' #{lz4_size_command} -o '#{product_object}'"

  sh "ld -r #{EXECUTABLE_OBJECT} -sectcreate __DATA __#{ext}_data '#{data_file}' -sectcreate __DATA __exec_cmd '#{tmp(exec_cmd)}' -sectcreate __DATA __uid '#{tmp(uid)}' -o '#{product_object}'"
  sh "clang #{product_object} -o #{product}"
end

desc 'Clean'
task :clean do
  rm_rf 'build'
end

desc 'Build and run'
task :run => :link do
  sh ENV['BINNAME']
end

#desc 'Build and run'
#task :run => :kisstribute do
  #product = ENV['BINNAME'] || raise('[!] Specify BINNAME env var.')
  #data_file = ENV['DATAFILE'] || raise('[!] Specify DATAFILE env var.')
  #exec_cmd = ENV['EXECCMD'] || raise('[!] Specify EXECCMD env var.')
  #sh "./#{FINAL_PRODUCT} #{data_file} #{product} '#{exec_cmd}'"
  #sh "./#{product}"
#end

ENV['BINNAME'] ||= File.join(PRODUCTS_DIR, 'test')
ENV['DATAFILE'] ||= 'fixtures/test.tar'
#ENV['DATAFILE'] ||= 'fixtures/test.tar.lz4'
#ENV['ORIGINALFILE'] ||= 'fixtures/test.tar'
ENV['EXECCMD'] ||= '/bin/ls -l'
ENV['UID'] ||= 'v1.0.0'

task :default => :run
