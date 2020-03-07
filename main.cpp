#include "ifcgeom/IfcGeomIterator.h"
#include "ifcgeom/IfcGeomRenderStyles.h"
#include "ifcparse/utils.h"
#include <Standard_Version.hxx>
#include <TopoDS_ListOfShape.hxx>
#include "GlfwOcctView.h"
typedef double real_t;
typedef std::string path_t;
static  std::ostream& cout_ = std::cout;
static  std::ostream& cerr_ = std::cerr;

int main(int argc, char** argv) {
	if ( argc != 2 ) {
        cout_ << "Usage: ifcviewer <input.ifc>\n"
            << "\n"
            << "Converts the geometry in an IFC file into glfw view\n";
        cout_ << std::endl;
		return 1;
	}
	IfcParse::IfcFile ifc_file;
	if ( ! ifc_file.Init(argv[1]) ) {
		std::cout << "Unable to parse .ifc file" << std::endl;
		return 1;
	}
    TopoDS_ListOfShape L;
    IfcGeom::IteratorSettings settings;
    std::vector<IfcGeom::filter_t> filter_funcs;
    std::set<std::string> entities;
    entities.insert("IfcSpace");
    entities.insert("IfcOpeningElement");
    IfcGeom::entity_filter entity_filter;
    entity_filter.populate(entities);
    filter_funcs.push_back(entity_filter); 
    settings.set(IfcGeom::IteratorSettings::DISABLE_TRIANGULATION, true);

    IfcSchema::IfcProduct::list::ptr products = ifc_file.entitiesByType<IfcSchema::IfcProduct>();
	std::cout << "Found " << products->size() << " products in " << argv[1] << ":" << std::endl;
	for ( IfcSchema::IfcProduct::list::it it = products->begin(); it != products->end(); ++ it ) {
		const IfcSchema::IfcProduct* product = *it;
        if (product->is(IfcSchema::Type::FromString("IFCOPENINGELEMENT"))) continue;
        if (product->hasRepresentation())
        {
            IfcGeom::Kernel kernel;
            IfcSchema::IfcProductRepresentation* prodrep = product->Representation();
			IfcSchema::IfcRepresentation::list::ptr reps = prodrep->Representations();
            for (IfcSchema::IfcRepresentation::list::it it = reps->begin(); it != reps->end(); ++it) {
                IfcSchema::IfcRepresentation* rep = *it;
                if (!rep->hasRepresentationIdentifier()) {
                    continue;
                }

                //同时需要考虑到find a representation within the 'Model' or 'Plan' context
                //todo: need concern about finding a representation within the 'Model' or 'Plan' context
                if (rep->RepresentationIdentifier() == "Body") {
		            //std::cout << "Body" << std::endl;
                    //todo : create_brep_for_representation_and_product<real_t> need debug
                    //要参考一下 ifcopenshell 0.6 这个函数的版本， 可能有解决方案？ 可能没有
                    //To refer to the version of ifcopenshell 0.6, may there be a solution? Maybe not
                    //ifcgeom/IfcGeomFunctions.cpp
                    IfcGeom::BRepElement<real_t>* o = kernel.create_brep_for_representation_and_product<real_t>(settings, 
                            rep, (IfcSchema::IfcProduct*)product);
                    for (IfcGeom::IfcRepresentationShapeItems::const_iterator it = o->geometry().begin(); it != o->geometry().end(); ++ it) {
                            gp_GTrsf gtrsf = it->Placement();
                            const gp_Trsf& o_trsf = o->transformation().data();
                            gtrsf.PreMultiply(o_trsf);
                            if (o->geometry().settings().get(IfcGeom::IteratorSettings::CONVERT_BACK_UNITS)) {
                                gp_Trsf scale;
                                scale.SetScaleFactor(1.0 / o->geometry().settings().unit_magnitude());
                                gtrsf.PreMultiply(scale);
                            }
                            const TopoDS_Shape& s = it->Shape();
                            const TopoDS_Shape moved_shape = IfcGeom::Kernel::apply_transformation(s, gtrsf);
                            L.Append(moved_shape);
                     }
                }
            }
		    std::cout << product->entity->toString() << std::endl;
        }
    }

    /*
    IfcGeom::Iterator<real_t> context_iterator(settings, &ifc_file, filter_funcs);
    if (!context_iterator.initialize()) {
        cerr_ << "No geometrical entities found" << std::endl;
        return EXIT_FAILURE;
    }
    cout_ << "Creating geometry..." << std::endl;
	size_t num_created = 0;
	do {
        IfcGeom::Element<real_t> *geom_object = context_iterator.get();
        const IfcGeom::BRepElement<real_t>* o = static_cast<const IfcGeom::BRepElement<real_t>*>(geom_object);
        for (IfcGeom::IfcRepresentationShapeItems::const_iterator it = o->geometry().begin(); it != o->geometry().end(); ++ it) {
                gp_GTrsf gtrsf = it->Placement();
                const gp_Trsf& o_trsf = o->transformation().data();
                gtrsf.PreMultiply(o_trsf);
                if (o->geometry().settings().get(IfcGeom::IteratorSettings::CONVERT_BACK_UNITS)) {
                    gp_Trsf scale;
                    scale.SetScaleFactor(1.0 / o->geometry().settings().unit_magnitude());
                    gtrsf.PreMultiply(scale);
                }
                const TopoDS_Shape& s = it->Shape();
                const TopoDS_Shape moved_shape = IfcGeom::Kernel::apply_transformation(s, gtrsf);
                L.Append(moved_shape);
         }
    } while (++num_created, context_iterator.next());
    cout_ << "Done creating geometry" << std::endl;
    */

    GlfwOcctView anApp(L);
    try
    {
        anApp.run();
    }
    catch (const std::runtime_error& theError)
    {
        cerr_ << theError.what() << std::endl;
        return EXIT_FAILURE;
    }
}

