function translate_mods_test()

    addpath('parsers');

    mod_names = {'hh', 'ionleak', 'kadist', 'kdrca1', 'na3n', 'naxn'};
    
    mod_dir = fullfile(cd, 'mods');
    
    n = length(mod_names);
    paths_to_mod = cell(n, 1);
    for i = 1 : n
        paths_to_mod{i} = fullfile(mod_dir, [mod_names{i}, '.mod']);
    end

    out_path = fullfile(cd, 'ModTranslatorTest', 'ModTranslatorTest');
        
    translate_mods(paths_to_mod, out_path);

end
