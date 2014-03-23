BUILD_DIR = 'build'
OBJECTS_BUILD_DIR = File.join(BUILD_DIR, 'base-objects')
EXECUTABLE_OBJECT = File.join(OBJECTS_BUILD_DIR, 'product-template.o')
PRODUCTS_DIR = File.join(BUILD_DIR, 'products')

desc 'Create stub'
task :stub do
  mkdir_p OBJECTS_BUILD_DIR
  sources = %w{ stub.c untar.c lz4/lz4.c }
  objects = sources.map do |source|
    File.join(OBJECTS_BUILD_DIR, "#{File.basename(source, '.c')}.o")
  end
  sources.zip(objects).each do |source, object|
    sh "clang -c #{source} -o #{object}"
  end
  sh "ld -r #{objects.join(' ')} -o #{EXECUTABLE_OBJECT}"
end

desc 'Link data file into final binary'
task :link => :stub do
  name = ENV['BINNAME'] || raise('[!] Specify BINNAME env var.')
  data = ENV['DATAFILE'] || raise('[!] Specify DATAFILE env var.')

  mkdir_p PRODUCTS_DIR
  object = File.join(PRODUCTS_DIR, "#{name}.o")
  sh "ld -r #{EXECUTABLE_OBJECT} -sectcreate __DATA __#{File.extname(data)[1..-1]}_data '#{data}' -o '#{object}'"
  sh "clang #{object} -o #{File.join(PRODUCTS_DIR, name)}"
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

task :default => :run
