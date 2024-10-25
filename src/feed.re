open Ppx_deriving_json_runtime.Primitives;

module Feed = {
  [@deriving json]
  type link = {
    href: string,
    title: string,
  };

  [@deriving json]
  type entry = {
    id: string,
    content: option(string),
    links: array(link),
    title: string,
    updated: float,
  };

  [@deriving json]
  type feed = {entries: array(entry)};
};
