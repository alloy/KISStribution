desc 'Create stub'
task :stub do
  mkdir_p 'build'
  sh "clang -c stub.c -o build/stub.o"
end

desc 'Link data file into final binary'
task :link => :stub do
  name = ENV['BINNAME'] || raise('[!] Specify BINNAME env var.')
  data = ENV['DATAFILE'] || raise('[!] Specify DATAFILE env var.')
  object = "build/#{name}.o"
  sh "ld -r build/stub.o -sectcreate __DATA __tar_data '#{data}' -o '#{object}'"
  #sh "clang '#{object}' -flat_namespace -undefined suppress -o 'build/#{name}'"
  sh "clang '#{object}' -g -o 'build/#{name}'"
end

desc 'Clean'
task :clean do
  rm_rf 'build'
end

desc 'Build and run'
task :run => :link do
  sh "./build/#{ENV['BINNAME']}"
end

task :default => :run